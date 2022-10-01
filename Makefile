UNAME=$(shell uname)
ifeq ($(UNAME), Windows)
	CROSS=x86_64-w64-mingw32.static-
endif

NAME=tentris
GAMDEPS=audio backdrop game graphics input screen sprite spritemap text util

SOURCES=$(wildcard *.cc) $(patsubst %,gam/%.cc,$(GAMDEPS))
RENDERS=$(patsubts resources/%.ase,content/%.png,$(wildcard resources/*.ase))
CONTENT=$(wildcard content/*) $(RENDERS)
ICONS=icon.png
BUILDDIR=$(CROSS)output
OBJECTS=$(patsubst %.cc,$(BUILDDIR)/%.o,$(SOURCES))
VERSION=$(shell git describe --tags --dirty)

CXX=$(CROSS)g++
LD=$(CROSS)ld
AR=$(CROSS)ar
PKG_CONFIG=$(CROSS)pkg-config
CPPFLAGS=-O3 --std=c++17 -Wall -Wextra -Werror -pedantic -I gam -DNDEBUG
EMFLAGS=-s USE_SDL=2 -s USE_SDL_MIXER=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE_OGG=1 -s USE_VORBIS=1 -s ALLOW_MEMORY_GROWTH=1 -fno-rtti -fno-exceptions
EXTRA=

EXECUTABLE=$(BUILDDIR)/$(NAME)

ifeq ($(UNAME), Windows)
	PACKAGE=$(NAME)-windows-$(VERSION).zip
	LDFLAGS=-static-libstdc++ -static-libgcc
	LDLIBS=`$(PKG_CONFIG) sdl2 SDL2_mixer SDL2_image --cflags --libs` -Wl,-Bstatic
	EXECUTABLE=$(BUILDDIR)/$(NAME).exe
	EXTRA=$(BUILDDIR)/icon.res.o
endif
ifeq ($(UNAME), Linux)
	PACKAGE=$(NAME)-linux-$(VERSION).AppImage
	LDFLAGS=-static-libstdc++ -static-libgcc
	LDLIBS=`$(PKG_CONFIG) sdl2 SDL2_mixer SDL2_image --cflags --libs` -Wl,-Bstatic
endif
ifeq ($(UNAME), Darwin)
	PACKAGE=$(NAME)-macos-$(VERSION).dmg
	LDLIBS=-framework SDL2 -framework SDL2_mixer -framework SDL2_image -rpath @executable_path/../Frameworks -F /Library/Frameworks/
	CPPFLAGS+=-mmacosx-version-min=10.9
endif

.PHONY: all echo clean distclean run package wasm web renders

all: $(EXECUTABLE)

echo:
	@echo "Renders: $(RENDERS)"
	@echo "Content: $(CONTENT)"
	@echo "Sources: $(SOURCES)"
	@echo "Uname: $(UNAME)"
	@echo "Package: $(PACKAGE)"
	@echo "Version: $(VERSION)"

run: $(EXECUTABLE)
	./$(EXECUTABLE)

renders: $(RENDERS)

content/%.png: resources/%.ase
	aseprite --batch $< --save-as $@

$(EXECUTABLE): $(OBJECTS) $(EXTRA) $(CONTENT)
	$(CXX) $(CPPFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(EXTRA) $(LDLIBS)

$(BUILDDIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c $(CPPFLAGS) -o $@ $<

package: $(PACKAGE)

$(BUILDDIR)/icon.res.o: $(BUILDDIR)/icon.rc
	$(CROSS)windres $< -O coff $@

$(BUILDDIR)/icon.rc: $(BUILDDIR)/icon.ico
	echo "420 ICON $<" > $@

$(BUILDDIR)/icon.ico: $(ICONS)
	convert $< $@

wasm: $(NAME)-$(VERSION).html

web: wasm $(NAME)-$(VERSION).js $(NAME)-$(VERSION).data
	mkdir -p $(NAME)-web-$(VERSION)
	cp $(NAME)-$(VERSION).js $(NAME)-web-$(VERSION)
	cp $(NAME)-$(VERSION).wasm $(NAME)-web-$(VERSION)
	cp $(NAME)-$(VERSION).data $(NAME)-web-$(VERSION)
	cp $(NAME)-$(VERSION).html $(NAME)-web-$(VERSION)/index.html

$(NAME)-macos-$(VERSION).dmg: $(NAME).app
	mkdir $(NAME)
	cp -r $(NAME).app $(NAME)/.
	hdiutil create tmp.dmg -ov -volname $(NAME) -fs HFS+ -srcfolder $(NAME)
	hdiutil convert tmp.dmg -format UDZO -o $@
	rm -rf $(NAME)
	rm tmp.dmg

$(NAME)-windows-$(VERSION).zip: $(EXECUTABLE) $(CONTENT)
	mkdir -p $(NAME)/content
	cp $(EXECUTABLE) $(NAME)/`basename $(EXECUTABLE)`
	cp $(CONTENT) $(NAME)/content/.
	zip -r $@ $(NAME)
	rm -rf $(NAME)

$(NAME)-$(VERSION).html: $(SOURCES) $(CONTENT)
	emcc $(CPPFLAGS) $(EMFLAGS) -o $@ $(SOURCES) --preload-file content/

$(NAME).app: $(EXECUTABLE) launcher $(CONTENT) Info.plist
	rm -rf $(NAME).app
	mkdir -p $(NAME).app/Contents/{MacOS,Frameworks}
	cp $(EXECUTABLE) $(NAME).app/Contents/MacOS/game
	cp launcher $(NAME).app/Contents/MacOS/launcher
	cp -R content $(NAME).app/Contents/MacOS/content
	cp Info.plist $(NAME).app/Contents/Info.plist
	cp -R /Library/Frameworks/SDL2.framework $(NAME).app/Contents/Frameworks/SDL2.framework
	cp -R /Library/Frameworks/SDL2_mixer.framework $(NAME).app/Contents/Frameworks/SDL2_mixer.framework
	cp -R /Library/Frameworks/SDL2_image.framework $(NAME).app/Contents/Frameworks/SDL2_image.framework

$(NAME)-linux-$(VERSION).AppDir: $(EXECUTABLE) $(CONTENT) AppRun icon.png $(NAME).desktop
	rm -rf $@
	mkdir -p $@/usr/{bin,lib}
	mkdir -p $@/content
	cp $(EXECUTABLE) $@/usr/bin
	cp AppRun $@/.
	cp $(NAME).desktop $@/.
	cp icon.png $@/.
	cp $(CONTENT) $@/content/.
	cp /usr/lib/libSDL2{,_image,_mixer}-2.0.so.0 $@/usr/lib/.

$(NAME)-linux-$(VERSION).AppImage: $(NAME)-linux-$(VERSION).AppDir
	ARCH=x86_64 appimagetool $< $@

clean:
	$(RM) -rf $(BUILDDIR)

distclean: clean
	$(RM) -rf *.app *.dmg *.zip
	$(RM) -rf *.AppDir *.AppImage
	$(RM) -rf *.html *.js *.data *.wasm
	$(RM) -rf *-web-*/ *output/
