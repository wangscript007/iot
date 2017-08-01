############################################################################################
#	common bits	used by	all	libraries
############################################################################################

SRC_DIR	=	"$(SRC_PATH_BARE)/lib$(NAME)"

CFLAGS	 +=	$(CFLAGS-yes)
OBJS		 +=	$(OBJS-yes)
ASM_OBJS +=	$(ASM_OBJS-yes)
CPPOBJS	 +=	$(CPPOBJS-yes)

CFLAGS +=	-I$(SRC_PATH)	\
					$(INCFLAGS)	$(OPTFLAGS)
CPPFLAGS +=	-I$(SRC_PATH)	\
					$(INCFLAGS)	$(OPTCPPFLAGS)
SRCS :=	$(OBJS:.o=.c)	$(ASM_OBJS:.o=.S)	$(CPPOBJS:.o=.cpp)
OBJS :=	$(OBJS)	$(ASM_OBJS)	$(CPPOBJS)
STATIC_OBJS	:= $(OBJS) $(STATIC_OBJS)
SHARED_OBJS	:= $(OBJS) $(SHARED_OBJS)

all: $(EXTRADEPS)	$(LIB) $(SLIBNAME) $(APPLICATION)

$(LIB):	$(STATIC_OBJS)
	rm -f	$@
	$(AR)	rc $@	$^ $(EXTRAOBJS)
	$(RANLIB)	$@

$(SLIBNAME): $(SLIBNAME_WITH_VERSION)
#	ln -sf $^	$@

$(SLIBNAME_WITH_VERSION):	$(SHARED_OBJS)
ifeq ($(BUILD_CPP),yes)
	$(CCC) $(SHFLAGS)	$(LDFLAGS) -o	$@ $^	$(EXTRALIBS) $(EXTRAOBJS)
else
	$(CC)	$(SHFLAGS) $(LDFLAGS)	-o $@	$^ $(EXTRALIBS)	$(EXTRAOBJS)
endif
	$(SLIB_EXTRA_CMD)

$(APPLICATION):	$(SHARED_OBJS)
ifeq ($(BUILD_CPP),yes)
	$(CCC) $(LDFLAGS)	-o $@	$^ $(EXTRALIBS)	$(EXTRAOBJS)
else
	$(CC)	$(LDFLAGS) -o	$@ $^	$(EXTRALIBS) $(EXTRAOBJS)
endif

%.o: %.cpp
	$(CCC) $(CPPFLAGS) $(LIBOBJFLAGS)	-c -o	$@ $<

%.o: %.c
	$(CC)	$(CFLAGS)	$(LIBOBJFLAGS) -c	-o $@	$<

%.o: %.S
	$(CC)	$(CFLAGS)	$(LIBOBJFLAGS) -c	-o $@	$<

%: %.o $(LIB)
ifeq ($(BUILD_CPP),yes)
	$(CCC) $(LDFLAGS)	-o $@	$^ $(EXTRALIBS)
else
	$(CC)	$(LDFLAGS) -o	$@ $^	$(EXTRALIBS)
endif

depend dep:	$(SRCS)
ifeq ($(BUILD_CPP),yes)
	$(CCC) -MM $(CPPFLAGS) $^	1>.depend
else
	$(CC)	-MM	$(CFLAGS)	$^ 1>.depend
endif

clean::
	rm -f	*.o	*.d	*~ *.a *.so	*.out	$(APPLICATION)

distclean: clean
	rm -f	.depend

ifeq ($(BUILD_SHARED),yes)
INSTLIBTARGETS +=	install-lib-shared
endif
ifeq ($(BUILD_STATIC),yes)
INSTLIBTARGETS +=	install-lib-static
endif

install: install-libs	install-headers

install-libs:	$(INSTLIBTARGETS)

install-lib-shared:	$(SLIBNAME)
	install	-d "$(shlibdir)"
	install	-m 755 $(SLIBNAME) "$(shlibdir)/$(SLIBNAME_WITH_VERSION)"
	$(STRIP) "$(shlibdir)/$(SLIBNAME_WITH_VERSION)"
	cd "$(shlibdir)" &&	\
		ln -sf $(SLIBNAME_WITH_VERSION)	$(SLIBNAME_WITH_MAJOR)
	cd "$(shlibdir)" &&	\
		ln -sf $(SLIBNAME_WITH_VERSION)	$(SLIBNAME)
	$(SLIB_INSTALL_EXTRA_CMD)

install-lib-static:	$(LIB)
	install	-d "$(libdir)"
	install	-m 644 $(LIB)	"$(libdir)"
	$(LIB_INSTALL_EXTRA_CMD)

install-headers:
	install	-d "$(incdir)"
	install	-d "$(libdir)/pkgconfig"
	install	-m 644 $(addprefix $(SRC_DIR)/,$(HEADERS)) "$(incdir)"
	install	-m 644 $(BUILD_ROOT)/lib$(NAME).pc "$(libdir)/pkgconfig"

uninstall: uninstall-libs	uninstall-headers

uninstall-libs:
	-rm	-f "$(shlibdir)/$(SLIBNAME_WITH_MAJOR)"	\
				 "$(shlibdir)/$(SLIBNAME)"						\
				 "$(shlibdir)/$(SLIBNAME_WITH_VERSION)"
	-rm	-f "$(libdir)/$(LIB)"

uninstall-headers:
	rm -f	$(addprefix	"$(incdir)/",$(HEADERS))
	rm -f	"$(libdir)/pkgconfig/lib$(NAME).pc"

.PHONY:	all	depend dep clean distclean install*	uninstall*

ifneq	($(wildcard	.depend),)
include	.depend
endif