git = git -c user.name="Auto" -c user.email="auto@auto.com" 

BUILDDIR = $(PWD)/build
SRCDIR = $(PWD)/src
LLVM_VERSION=4.0.0

all : release

.PHONY : release debug run clean patch test

release : $(BUILDDIR)/buildr/Makefile
	+make -C $(BUILDDIR)/buildr
	cp -f $(BUILDDIR)/buildr/vts-synth vts-synth

debug : $(BUILDDIR)/buildd/Makefile
	+make -C $(BUILDDIR)/buildd
	cp -f $(BUILDDIR)/buildd/vts-synth vts-synth

$(BUILDDIR)/buildr/Makefile: $(BUILDDIR)/z3/buildr/libz3.so
	mkdir -p $(BUILDDIR)/buildr
	cd $(BUILDDIR)/buildr; cmake -DCMAKE_BUILD_TYPE=Release $(SRCDIR)

$(BUILDDIR)/buildd/Makefile: $(BUILDDIR)/z3/buildd/libz3.so
	mkdir -p $(BUILDDIR)/buildd
	cd $(BUILDDIR)/buildd; cmake -DCMAKE_BUILD_TYPE=Debug -DLLVM_VERSION=$(LLVM_VERSION) -DZ3_DEBUG:BOOL=TRUE $(SRCDIR)

clean :
	rm -rf $(BUILDDIR)/buildr
	rm -rf $(BUILDDIR)/buildd
	rm -f vts-synth
	find -name "*~"| xargs rm -rf


#-----------------------------------------------------------------------------
# Z3 fetch and compile


$(BUILDDIR)/z3/README.md : 
	mkdir -p $(BUILDDIR)
	# todo: switch to https; someone may not have ssh access configured
	cd $(BUILDDIR);$(git) clone git@github.com:Z3Prover/z3.git
	# cd $(BUILDDIR)/z3;$(git) checkout b8716b333908273ad8e27e325a8bea9be0596be3


$(BUILDDIR)/z3/buildr/Makefile: $(BUILDDIR)/z3/README.md
	rm -rf $(BUILDDIR)/z3/buildr
	cd $(BUILDDIR)/z3; python scripts/mk_make.py --staticlib -b buildr

$(BUILDDIR)/z3/buildr/libz3.so : $(BUILDDIR)/z3/buildr/Makefile
	+make -C $(BUILDDIR)/z3/buildr

$(BUILDDIR)/z3/buildr/libz3.a : $(BUILDDIR)/z3/buildr/libz3.so

$(BUILDDIR)/z3/buildd/Makefile: $(BUILDDIR)/z3/README.md
	rm -rf $(BUILDDIR)/z3/buildd
	cd $(BUILDDIR)/z3; python scripts/mk_make.py --staticlib -d -b buildd

$(BUILDDIR)/z3/buildd/libz3.so : $(BUILDDIR)/z3/buildd/Makefile
	+make -C $(BUILDDIR)/z3/buildd


#---------------------------------------------------------------------------
# runtest:
# 	make -C examples/tester/

# test: release runtest
