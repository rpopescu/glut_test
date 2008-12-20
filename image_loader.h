#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <fstream>
#include <iostream>
#include <map>
#include <utility>

using namespace std;

class image
{
public:
	image(const string& image_id, uint16_t w, uint16_t h, uint8_t bpp) : id(image_id), width(w), height(h), bpp(bpp), data(0)
	{
		data = new uint8_t[width*height*(bpp/8)];
	}
	~image()
	{
		if(data) delete [] data;
		data = NULL;
	}

	const string id;
	const uint16_t width, height;
	const uint8_t bpp;
	uint8_t* data;
private:	
	image(const image&);
	image& operator=(const image&);
};


struct image_loader
{
	static image* load(const string& fname)
	{
		ifstream ifs(fname.c_str(), ios::binary);
		if(!ifs) return NULL;
		uint8_t hdr[18];
		ifs.read((char*)hdr, 18);
		if(ifs.fail() || ifs.eof()) return NULL;
		if(hdr[1]) return NULL;
		uint8_t type = hdr[2];
		uint8_t bpp = hdr[16];
		enum TGAImageTypes
		{
			TGA_IMG_NONE = 0,
			TGA_IMG_COLORMAP = 1,
			TGA_IMG_TRUECOLOR = 2,
			TGA_IMG_BW = 3,
			TGA_IMG_RLE_COLORMAP = 9,
			TGA_IMG_RLE_TRUECOLOR = 10,
			TGA_IMG_RLE_BW = 11
		};
		if(type != TGA_IMG_TRUECOLOR && type != TGA_IMG_RLE_TRUECOLOR)
		{
			cout << "unsupported TGA image type (" << type << ")" << endl;
			return NULL;
		}
		if(bpp != 32 && bpp != 24)
		{
			cout << "unsupported TGA bpp value (" << bpp << ")" << endl;
			return NULL;
		}
		image* img = new image(fname, *((uint16_t*)(hdr + 12)), *((uint16_t*)(hdr + 14)), bpp);
		return (type == 2 ? decode_tga_plain(ifs, img) : decode_tga_rle(ifs, img));
	}

private:
	static image* decode_tga_plain(ifstream& ifs, image* img)
	{
		ifs.read((char*)(img->data), img->width * img->height * (img->bpp / 8));
		return img;
	}

	static image* decode_tga_rle(ifstream& ifs, image* img)
	{
		int i = 0;
		for(; i < img->width * img->height;)
		{
			uint8_t repcount = 0;
			uint32_t pixel = 0;
			ifs.read((char*)&repcount, 1);
			if(ifs.fail() || ifs.eof())
			{
				cout << "error reading pixel data" << endl;
				delete img;
				return NULL;
			}
			if(repcount >= 0x80) // last bit set, RLE packet
			{
				repcount &= 0x7F; // clear the first bit, obtain actual repetition count
				repcount++;
				assert(repcount > 0);
				ifs.read((char*)&pixel, img->bpp/8); // read pixel value
				for(int r = 0; r < repcount; r++) // paste the pixel repcount times
				{
					memcpy(&img->data[(i + r)*(img->bpp/8)], (char*)&pixel, img->bpp/8);
				}
				i += repcount; // have processed repcount pixels
			}
			else // last bit not set, RAW packet
			{
				repcount++; // it's one less than the actual number of pixels to follow
				for(int r = 0; r< repcount; r++)
				{
					ifs.read((char*)&pixel, img->bpp/8); // read pixel value
					memcpy(&img->data[(i + r)*(img->bpp/8)], (char*)&pixel, img->bpp/8);
				}
				i += repcount;
			}
		}
		assert(i == img->width * img->height);
		return img;
	}
};


struct image_manager
{
	static image* get(const string& fname)
	{
		map<string, image*>::iterator i = image_map.find(fname);
		if(i == image_map.end())
		{
			cout << "image_manager: loading " << fname << endl;
			image* img = image_loader::load(fname);
			cout << "img [" << img->id << "] : " << img->width << "x" << img->height << endl;
			image_map.insert(make_pair(fname, img));
			return img;
		}
		else
		{
			return (*i).second;
		}
	}
	static void dump(const string&fname, const string& fname2)
	{
		image* img = get(fname);
		if(!img) return;
		ofstream ofs(fname2.c_str(), ios::binary);
		ofs.write((const char*)img->data, img->width * img->height * (img->bpp/8));
		ofs.close();
	}
	static map<string, image*> image_map;
};

map<string, image*> image_manager::image_map;
#endif

