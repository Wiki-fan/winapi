#pragma once
#define ERRMSG(TEXT, MSG) (TEXT+std::to_string(MSG)).c_str()

class Field {
public:
	int width, height;

};
