from PIL import Image
import sys
filename = sys.argv[1]
filename = filename[:filename.find('.png')]
print('convert',filename,'to .tif')
# filename = 't50'
im = Image.open(filename+'.png')

im.save("_"+filename+'.tif', format='TIFF',\
dpi=(100., 100.), compression='tiff_lzw',)

im.save("../../../Multicolor/build/_"+filename+'.tif', format='TIFF',\
dpi=(100., 100.), compression='tiff_lzw',)

