#pragma once

namespace dx11 {

///================================================================================= FontChar
struct FontChar final {
	uint id;
	float u, v, u2, v2;
	uint width, height;
	int xoffset, yoffset;
	uint xadvance;
};
///================================================================================= FontPage
class FontPage final {
public:
	unordered_map<uint, FontChar> chars;
	unordered_map<ulong, int> kernings; /// key = (from<<32 | to)

	int getKerning(uint from, uint to) const {
		auto iter = kernings.find(((ulong)from) << 32 | to);
		if(iter == kernings.end()) return 0;
		return iter->second;
	}
	const FontChar& operator[](uint ch) {
		auto iter = chars.find(ch);
		if(iter==chars.end()) return chars[' '];
		return iter->second;
	}
};
///================================================================================= Font
class Font final {
public:
	FontPage page;
	wstring name;
	uint size, width, height, lineHeight;
	Texture2D texture;

	int getKerning(uint from, uint to) const { 
		return page.getKerning(from, to);
	}
	const FontChar& getChar(uint ch) {
		return page[ch];
	}
	float2 getDimension(const string& text, float size) { return getRect(text, size).dimension(); }
	Rect getRect(const string& text, float size);
};
///================================================================================= Fonts
class Fonts final {
	wstring directory = L"./";
	unordered_map<wstring, unique_ptr<Font>> fonts;
	DX11& dx11;
public:
	Fonts(DX11& dx11) : dx11(dx11) {}
	Font* get(const wstring& name);
	void setDirectory(const wstring& dir) { this->directory = dir; }
private:
	void readFontPage(Font& font);
	void readFontTexture(Font& font);
};
///================================================================================= 

} /// dx11