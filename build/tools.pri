
include (./path.pri)
ky2ToolsPath = $${ky2SourcePath}/tools

Image_Header = \
    $${ky2ToolsPath}/image/impl_image_scaled.h
Image_Source = \
    $${ky2ToolsPath}/image/impl_image_scaled.cpp \
    $${ky2ToolsPath}/image/impl_pixel_info.cpp

DEFINES += STB_IMAGE_STATIC
DEFINES += STB_IMAGE_WRITE_STATIC
DEFINES += STB_IMAGE_RESIZE_STATIC
DEFINES += STB_IMAGE_IMPLEMENTATION
DEFINES += STB_IMAGE_WRITE_IMPLEMENTATION
DEFINES += STB_IMAGE_RESIZE_IMPLEMENTATION

HEADERS += \
    $${Image_Header} \
    $${ky2ToolsPath}/unicode_table.h \
    $${ky2ToolsPath}/ky_ucsprivate.h \
    $${ky2ToolsPath}/ky_ucs.h \
    $${ky2ToolsPath}/ky_unicode.h \
    $${ky2ToolsPath}/ky_number_integer.h \
    $${ky2ToolsPath}/ky_number_double.h

SOURCES += \
    $${Image_Source} \
    $${ky2ToolsPath}/ky_algorlthm.cpp \
    $${ky2ToolsPath}/ky_hash.cpp \
    $${ky2ToolsPath}/ky_linked.cpp \
    $${ky2ToolsPath}/ky_list.cpp \
    $${ky2ToolsPath}/ky_rbtree.cpp \
    $${ky2ToolsPath}/ky_map.cpp \
    $${ky2ToolsPath}/ky_utils.cpp \
    $${ky2ToolsPath}/ky_uuid.cpp \
    $${ky2ToolsPath}/ky_variant.cpp \
    $${ky2ToolsPath}/ky_codec.cpp \
    $${ky2ToolsPath}/ky_char.cpp \
    $${ky2ToolsPath}/ky_unicode.cpp \
    $${ky2ToolsPath}/ky_string.cpp \
    $${ky2ToolsPath}/ky_path.cpp \
    $${ky2ToolsPath}/ky_stream.cpp \
    $${ky2ToolsPath}/ky_color.cpp \
    $${ky2ToolsPath}/ky_image.cpp \
    $${ky2ToolsPath}/ky_date.cpp \
    $${ky2ToolsPath}/ky_time.cpp
