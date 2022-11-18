set(srcs "main.c"
	"st7735s.c"
	"axp192.c"
	"fontx.c"
	"decode_jpeg.c"
	"decode_png.c"
	"pngle.c"
	)

idf_component_register(SRCS ${srcs} INCLUDE_DIRS ".")
