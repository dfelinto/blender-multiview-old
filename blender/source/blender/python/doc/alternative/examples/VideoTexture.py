import VideoTexture
import bge.logic

contr = bge.logic.getCurrentController()
obj = contr.owner

# the creation of the texture must be done once: save the 
# texture object in an attribute of bge.logic module makes it persistent
if not hasattr(bge.logic, 'video'):

	# identify a static texture by name
	matID = VideoTexture.materialID(obj, 'IMvideo.png')
	
	# create a dynamic texture that will replace the static texture
	bge.logic.video = VideoTexture.Texture(obj, matID)
	
	# define a source of image for the texture, here a movie
	movie = bge.logic.expandPath('//trailer_400p.ogg')
	bge.logic.video.source = VideoTexture.VideoFFmpeg(movie)
	bge.logic.video.source.scale = True
	
	# quick off the movie, but it wont play in the background
	bge.logic.video.source.play()	
	
	# you need to call this function every frame to ensure update of the texture.
	bge.logic.video.refresh(True)
 
