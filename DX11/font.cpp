#include "_pch.h"
#include "_exported.h"

namespace dx11 {

using namespace core;

const constexpr uint BM_WIDTH = 512;

static FontChar readChar(const string& lineIn) {
	FontChar c = {};
	string line = String::trimBoth(lineIn);
	// assumes there are no spaces around '='
	auto tokens = String::split(line);
	unordered_map<string, string> map;
	for(auto& it : tokens) {
		auto pair = String::split(it, '=');
		map[pair[0]] = pair[1];
	}
	c.id = String::toInt(map["id"]);
	float x = String::toFloat(map["x"]);
	float y = String::toFloat(map["y"]);
	c.width = String::toInt(map["width"]);
	c.height = String::toInt(map["height"]);
	c.xoffset = String::toInt(map["xoffset"]);
	c.yoffset = String::toInt(map["yoffset"]);
	c.xadvance = String::toInt(map["xadvance"]);
	c.u = x / BM_WIDTH;
	c.v = y / BM_WIDTH;
	c.u2 = (x + c.width - 1) / BM_WIDTH;
	c.v2 = (y + c.height - 1) / BM_WIDTH;
	return c;
}

Rect Font::getRect(const string& text, float size) {
	Rect r = {};
	if(text.size() == 0) return r;
	r.x = 1000;
	r.y = 1000;
	float X = 0;
	int i = 0;
	for(auto ch : text) {
		auto g		= getChar(ch); 
		float ratio = (size / (float)this->size);

		float x = X + g.xoffset * ratio;
		float y = 0 + g.yoffset * ratio;
		float xx = x + g.width * ratio;
		float yy = y + g.height * ratio;

		if(x<r.x) r.x = x;
		if(y<r.y) r.y = y;
		if(xx > r.width) r.width = xx;
		if(yy > r.height) r.height = yy;

		int kerning = 0;
		if(i+1<text.size()) {
			kerning = getKerning(ch, text[i + 1]);
		}
		X += (g.xadvance + kerning) * ratio;
	}
	return r;
}

shared_ptr<Font> Fonts::get(const wstring& name) {
	auto iter = fonts.find(name);
	if(iter != fonts.end()) {
		return iter->second;
	}
	Font font = {};
	font.name = name;
	readFontPage(font);
	readFontTexture(font);
	Log::format("Loaded font %s%s", WString::toString(directory).c_str(), WString::toString(name).c_str());

	fonts[name] = std::make_shared<Font>(font);
	return fonts[name];
}
void Fonts::readFontPage(Font& font) {
	const auto getFirstToken = [](string& line, ulong offset=0)->string { 
		auto p = offset;
		while(p<line.size() && line[p]>32) p++;
		return line.substr(offset, p - offset);
	};
	const auto getInt = [&](string& line, string&& key)->int {
		auto p = line.find(key);
		auto token = getFirstToken(line, p+key.size());
		return String::toInt(token);
	};

	FileReader<4096> reader{directory + font.name + L".fnt"};
	while(!reader.eof()) {
		string line = String::trimBoth(reader.readLine());
		if(line.size() == 0) continue;
		const auto firstToken = getFirstToken(line);
		//Log::format("firstToken='%s'", firstToken.c_str());

		if(firstToken == "char") {
			auto fc = readChar(line.substr(4));
			font.page.chars[fc.id] = fc;
		} else if(firstToken == "kerning") {
			ulong first = getInt(line, "first=");
			ulong second = getInt(line, "second=");
			int amount = getInt(line, "amount=");
			font.page.kernings[(first << 32) | second] = amount;
		} else if(firstToken == "info") {
			font.size = getInt(line, "size=");
		} else if(firstToken == "common") {
			font.width = getInt(line, "scaleW=");
			font.height = getInt(line, "scaleH=");
			font.lineHeight = getInt(line, "lineHeight=");
		}
	}
}
void Fonts::readFontTexture(Font& font) {
	wstring filename = directory + font.name + L".png";
	string filenameA = WString::toString(filename);
	int x, y, n;
	ubyte* data = stbi_load(filenameA.c_str(), &x, &y, &n, 0);
	if(!data) throw std::runtime_error("Texture load error: '" + filenameA + "'");

	assert(n == 4);
	/// Convert RGBA to single R channel
	ubyte* src = data+3;
	ubyte* dest = data;
	for(int i = 0; i < x*y; i++) {
		*dest = *src;
		dest++;
		src += 4;
	}

	font.texture.init(dx11.device, {BM_WIDTH, BM_WIDTH}, DXGI_FORMAT::DXGI_FORMAT_R8_UNORM, data);
	
	free(data);
}

} /// dx11