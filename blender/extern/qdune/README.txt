This is the initial commit of the 'quietdune' micropolygon renderer.
This is very much a work in progess, don't expect things to work properly or even at all... use at your own risk.
The SConscript is extremely simplistic and needs to be adapted for other systems to be able to compile sucessfully.
The renderer currently mostly uses fixed paths for shaders and the subdivision .dat files.
For this to work properly, the renderer expects to be started from the main 'qdune' directory.

For display output the CImg library is used. It is a very nice general image processing library by David Tschumperlé, but is only used as framebuffer here.
The program can work without it, but then always saves result images as file.
If anyone wants to recreate the full standalone program, get the CImg.h file here: http://cimg.sourceforge.net/ , and copy the file to the 'framebuffer' directory.

The subdivision surface is partially evaluated directly using the method from the paper "Exact Evaluation of Catmull-Clark Subdivision Surfaces at Arbitrary Parameter Values"
see http://www.dgp.toronto.edu/people/stam/reality/Research/SubdivEval/index.html
The .dat binary files in the main 'qdune' directory are used for this method.
Thanks to Jos Stam and Alias|Wavefront for making them available.

If you want to experiment with shaders, then you also need the ply parser available here:
http://www.dabeaz.com/ply/
The compiler is currently entirely python based, and needs at least python2.4 to work properly.
The 'cpp' preprocessor is expected to be available on your system, but may work without it so long as you don't use include files or macros in your shaders.
The compiler uses fixed paths as well, if you are going to use it, it is best to just copy the entire 'slcompiler' directory into the ply-2.3/example directory and work from there.
The main compiler is 'qdslc.py', simple usage: "python qdslc.py shadername.sl" (the -S and -O options can be used to optimize the code a bit and also make it a bit more 'readable').
If succesful a 'shadername.sqd' file will be the result, copy it to the qdune/shaders directory.
The compiler is a very recent addition, so again, don't expect things to work well.

There are several example ribfiles in the 'ribs' directory which might be useful for testing.
The fig.12. ribfiles are example files from the 'Advanced Renderman' book, chapter 12, see http://www.renderman.org/RMR/Publications/index.html#Advanced for more info.
'suzgallery.rib' is a modified version of an example file from the well-known BMRT renderer, 'vasegallery1.rib'.

Anyone wishing to do work on the renderer will find the book 'Production Rendering' a very useful reference, since the entire design of qdune is based on it.
see http://www.amazon.com/Production-Rendering-Ian-Stephenson/dp/1852338210
