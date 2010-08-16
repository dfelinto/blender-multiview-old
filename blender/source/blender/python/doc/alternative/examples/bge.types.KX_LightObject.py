# Turn on a red alert light.
import bge

co = bge.logic.getCurrentController()
light = co.owner

light.energy = 1.0
light.colour = [1.0, 0.0, 0.0]
 
