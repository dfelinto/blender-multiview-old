///---------------------------------------------------------------------------------------
/// mktex, program to convert image textures to an 'optimized' tiled mip/ripmap format.
/// Currently only mipmaps supported though.
/// Also currently only handles jpeg or exr files, other image formats tiff/hdr/tga TODO
/// or maybe use some image library.
///---------------------------------------------------------------------------------------

#include <iostream>
#include <cstring>

#include "maketexture.h"
#include "Mathutil.h"

using namespace std;

static struct
{
	const char *name, *defval;
}
opts[] =
{{"-swrap", "black"},
 {"-twrap", "black"},
 {"-filter", "mitchell"},
 {"-swidth", "2"},
 {"-twidth", "2"},
 {"-half",   "true"},
 {"-gray",   "false"}
};

int main(int argc, char* argv[])
{
	if (argc == 1) {
		cout << "Usage: " << argv[0] << " [options] inputfile [outputfile]\n";
		cout << "\noptions:\n";
		cout << "-swrap : wrap mode in s direction, [black(default), periodic, clamp]\n";
		cout << "-twrap : wrap mode in t direction, [black(default), periodic, clamp]\n";
		cout << "-filter: [box, triangle, catmull-rom, b-spline,\n          gaussian, sinc, lanczos, mitchell(default)]\n";
		cout << "-swidth: filter width in s direction\n";
		cout << "-twidth: filter width in t direction\n";
		cout << "-half  : [true(default), false] save float images in 'half' format\n";
		cout << "         can significantly reduce filesize.\n";
		cout << "-gray  : [true, false(default)] if the image is in fact a grayscale image\n";
		cout << "         that for some reason is in RGB format, set this to true to force\n";
		cout << "         mktex to save only the R channel as single channel image instead.\n";
		cout << "The input image types currently supported are jpeg and exr.\n";
		cout << "If 'outputfile' is not given, the name of the output will be the name of the\ninput file plus the extension '.tqd'\n";
		cout << "The output file is a tiled multi-resolution OpenEXR image which can be viewed\nwith the 'exrdisplay' program.\n";
		cout << "If the inputfile is already an exr image, it is also possible to use\n";
		cout << "the OpenEXR program 'exrmaketiled' which will produce compatible files.\n\n";
		return 0;
	}
	const int optsize = sizeof(opts)/sizeof(opts[0]);
	int curarg = 1;
	const char* infile = NULL;
	const char* outfile = NULL;
	bool swidth_set = false, twidth_set = false;
	while (curarg < argc) {
		if (argv[curarg][0] == '-') {
			bool found = false;
			for (int i=0; i<optsize; ++i)
				if (!strcmp(argv[curarg], opts[i].name)) {
					opts[i].defval = argv[curarg + 1];
					if (i == 3) swidth_set = true; else if (i == 4) twidth_set = true;
					found = true;
					break;
				}
			if (!found) {
				cout << "Unknown option \"" << argv[curarg] << "\"\n";
				return 0;
			}
			curarg += 2;
		}
		else if (infile) {
			// outputfile, last argument
			outfile = argv[curarg];
			break;
		}
		else {
			infile = argv[curarg];
			curarg++;
			continue;
		}
	}

	// set default filter widths if not specified
	if (!swidth_set || !twidth_set) {
		if (!strcmp(opts[2].defval, "box")) {
			if (!swidth_set) opts[3].defval = "0.5";
			if (!twidth_set) opts[4].defval = "0.5";
		}
		else if (!strcmp(opts[2].defval, "triangle")) {
			if (!swidth_set) opts[3].defval = "1";
			if (!twidth_set) opts[4].defval = "1";
		}
		else if (!strcmp(opts[2].defval, "catmull-rom")) {
			if (!swidth_set) opts[3].defval = "2";
			if (!twidth_set) opts[4].defval = "2";
		}
		else if (!strcmp(opts[2].defval, "b-spline")) {
			if (!swidth_set) opts[3].defval = "2";
			if (!twidth_set) opts[4].defval = "2";
		}
		else if (!strcmp(opts[2].defval, "mitchell")) {
			if (!swidth_set) opts[3].defval = "2";
			if (!twidth_set) opts[4].defval = "2";
		}
		else if (!strcmp(opts[2].defval, "sinc")) {
			if (!swidth_set) opts[3].defval = "4";
			if (!twidth_set) opts[4].defval = "4";
		}
		else if (!strcmp(opts[2].defval, "lanczos")) {
			if (!swidth_set) opts[3].defval = "3";
			if (!twidth_set) opts[4].defval = "3";
		}
		else if (!strcmp(opts[2].defval, "gaussian")) {
			if (!swidth_set) opts[3].defval = "1.5";
			if (!twidth_set) opts[4].defval = "1.5";
		}
		else {
			cout << "Unknown filter: \"" << opts[2].defval << "\"\n";
			// use mitchell default
			opts[2].defval = "mitchell";
			if (!swidth_set) opts[3].defval = "2";
			if (!twidth_set) opts[4].defval = "2";
		}
	}

	char outname[256] = {0};
	if (infile && (outfile == NULL)) {
		const long int ps = QDRender::MIN2(int(strrchr(infile, '.') - infile), 254);
		strncpy(outname, infile, ps);
		outname[ps] = 0;
		strcat(outname, ".tqd");
		outfile = outname;
	}

	cout << "Option values:\n";
	for (int i=0; i<optsize; ++i)
		cout << opts[i].name << ": " << opts[i].defval << endl;
	cout << "infile : " << (infile ? infile : "none") << endl;
	cout << "outfile: " << (outfile ? outfile : "none") << endl;

	// must have at least outfile name (so also infile, see above)
	if (outfile) {
		bool half = (!strcmp(opts[5].defval, "true"));
		bool gray = (!strcmp(opts[6].defval, "true"));
		// create the texture file
		QDRender::makeTexture(infile, outfile, opts[0].defval, opts[1].defval, opts[2].defval,
			atof(opts[3].defval), atof(opts[4].defval), 0, NULL, NULL, half, gray);
	}
	else
		cout << "No files specified!\n";

	cout << "Done\n";
	return 0;
}
