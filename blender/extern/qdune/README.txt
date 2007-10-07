This is the initial commit of the 'quietdune' micropolygon renderer.
This is very much a work in progess, don't expect things to work properly or even at all... use at your own risk.
The SConscript is extremely simplistic and needs to be adapted for other systems to be able to compile sucessfully.
The renderer currently mostly uses fixed paths for shaders and the subdivision .dat files.
For this to work properly, the renderer expects to be started from the main 'qdune' directory.

There are several external files used here which are included:

First is the CImg.h include file in the 'framebuffer' directory, see http://cimg.sourceforge.net/
the CImg library by David Tschumperlé is a very nice general image processing library, but is only used as framebuffer here.

The subdivision surface is partially evaluated directly using the method from the paper "Exact Evaluation of Catmull-Clark Subdivision Surfaces at Arbitrary Parameter Values"
see http://www.dgp.toronto.edu/people/stam/reality/Research/SubdivEval/index.html
The .dat binary files in the main 'qdune' directory are used for this method.
Thanks to Jos Stam and Alias|Wavefront for making them available.

If you want to experiment with shaders, then you also need the ply parser available here:
http://www.dabeaz.com/ply/
The compiler is currently entirely python based, and needs at least python2.4 to work properly.
The 'cpp' preprocessor is expected to be available on your system, but may work without it so long as you don't use include files or macros in your shaders.
The compiler uses fixed paths as well, if you are going to use it, it is best to just copy the entire 'slcompiler' directory into the ply-2.3/example directory and work from there.
The main compiler is 'qdslc.py', usage: "python qdslc.py shadername.sl", if succesful a shadername.sqd file will be the result, copy it to the qdune/shaders directory.
The compiler is an extremely recent addition, so again, don't expect things to work well.

There are several example ribfiles in the 'ribs' directory which might be useful for testing.
The fig.12. ribfiles are from example files from the 'Advanced Renderman' book, see http://www.renderman.org/RMR/Publications/index.html#Advanced for more info.

Anyone wishing to do work on the renderer will find the book 'Production Rendering' a very useful reference, since the entire design of qdune is based on it.
see http://www.amazon.com/Production-Rendering-Ian-Stephenson/dp/1852338210
