#include "engine/Font.h"
#include "schrift.h"
#include "stb/rect_pack.h"
#include "math.h"

#include "stb/image_write.h"

Font::Font(string filePath, float scale) {

	SFT sft;

	sft.flags = SFT_DOWNWARD_Y;
	sft.yScale = scale;
	sft.xScale = scale;

	sft.font = sft_loadfile(filePath.c_str());

	SFT_LMetrics fontMetrics;
	sft_lmetrics(&sft, &fontMetrics);

	lineGap = fontMetrics.lineGap;
	ascender = fontMetrics.ascender;
	descender = fontMetrics.descender;

	map<char, vector<uint8_t>> bitmaps;
	int neededPixels = 0;

	for(unsigned char c = 0; c < 128; c++) {
		SFT_Glyph glyph;
		if(sft_lookup(&sft, c, &glyph) < 0)
			throw runtime_error("debuggo");

		SFT_GMetrics metrics;
		sft_gmetrics(&sft, glyph, &metrics);

		SFT_Image img;
		img.width = (metrics.minWidth + 3) & ~3; // 4px aligned
		img.height = metrics.minHeight;

		vector<uint8_t> pixels(img.width * img.height);
		
		neededPixels += img.width * img.height;

		img.pixels = pixels.data();
		sft_render(&sft, glyph, img);

		glyphs[c] = Glyph{
			iV2D{0,0}, // Texture Coords - will be set later.
			iV2D{ // size
				img.width,
				img.height
			},
			iV2D{ // bearing
				(int)metrics.leftSideBearing,
				(int)metrics.yOffset
			},
			(uint32_t)metrics.advanceWidth,
		};

		bitmaps[c] = pixels;

	}

	// start stitching bitmap here;
	
	stbrp_context ctx;

	stbrp_node nodes[bitmaps.size()];
	vector<stbrp_rect> rects;

	rects.reserve(bitmaps.size());

	for(auto& pair : glyphs) {

		stbrp_rect r;

		r.id = pair.first;
		r.w = pair.second.size.x;
		r.h = pair.second.size.y;
		r.was_packed = 0;

		rects.push_back(r);

	}

	unsigned int v = sqrt(neededPixels);
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;

	int atlasW = v*2; // add space for packing
	int altasH = atlasW;

	bitmap = vector<uint8_t>(atlasW*altasH);
	size = { atlasW, altasH };

	stbrp_init_target(&ctx, atlasW, altasH, nodes, bitmaps.size());
	stbrp_pack_rects(&ctx, rects.data(), bitmaps.size());

	for(auto& rect : rects) {
		if(!rect.was_packed) throw runtime_error("failed to pack glyph");

		auto& glyphBitmap = bitmaps[rect.id];

		glyphs[rect.id].texCoords = { rect.x, rect.y };

		for(int y = 0; y < rect.h; y++) {
			for(int x = 0; x < rect.w; x++) {
				bitmap[(atlasW * (y + rect.y)) + (x + rect.x)] = glyphBitmap[(rect.w * y) + x];
			}
		}
	}

	stbi_write_bmp("debug.bmp", atlasW, altasH, 1, bitmap.data());

	sft_freefont(sft.font);
}
