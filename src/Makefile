# ----------------------------------------------------
# Target Specs
# ----------------------------------------------------
DIST_C_MODULES = hello # fake name as it cannot be an empty string
DIST_C_FILES = $(addsuffix .c, $(DIST_C_MODULES))
DIST_OBJ_FILES = $(addsuffix .o, $(addprefix $(COBJDIR)/, $(DIST_C_MODULES)))
DIST_BIN_FILES = $(addprefix $(CBINDIR)/, $(DIST_C_MODULES))

NOLINK_DIST_C_MODULES = object exception logger linked_list counter root main #bdb
NOLINK_DIST_C_FILES = $(addsuffix .c, $(NOLINK_DIST_C_MODULES))
NOLINK_DIST_OBJ_FILES = $(addsuffix .o, \
	$(addprefix $(COBJDIR)/, $(NOLINK_DIST_C_MODULES)))
NOLINK_DIST_BIN_FILES = $(addprefix $(CBINDIR)/, $(NOLINK_DIST_C_MODULES))

TARGET_FILES = throw_test
BIN_TARGETS = $(TARGET_FILES:%=$(CBINDIR)/%)

TEST_C_MODULES = #sum
TEST_C_FILES = $(addsuffix _test.c, $(TEST_C_MODULES))
TEST_OBJ_FILES = $(addsuffix .to, \
	$(addprefix $(COBJDIR)/, $(TEST_C_MODULES)))
TEST_BIN_FILES = $(addsuffix .test, \
	$(addprefix $(TESTDIR)/, $(TEST_C_MODULES)))
TEST_ENV_MODULES = object exception logger linked_list counter root
TEST_ENV_OBJ_FILES = $(addsuffix .o, \
	$(addprefix $(COBJDIR)/, $(TEST_ENV_MODULES)))

# ----------------------------------------------------
# Includes and libs
# ----------------------------------------------------

TYPE_FLAGS = -Wall -O2

#PRIVDIR = ../.install
PRIVDIR = $(INSTALL_ROOT)
CBINDIR = $(PRIVDIR)/bin
COBJDIR = $(PRIVDIR)/obj
CLIBDIR = $(PRIVDIR)/lib
TESTDIR = $(PRIVDIR)/test

# ----------------------------------------------------
# File suffixes
# ----------------------------------------------------
exe =
#exe = .so
obj = .o

# ----------------------------------------------------
# Common Macros
# ----------------------------------------------------
INSTALL_DIR = mkdir -p

# ----------------------------------------------------
# Targets
# ----------------------------------------------------
#show: 
#	echo NOLINK_DIST_OBJ_FILES = $(NOLINK_DIST_OBJ_FILES) : $(NOLINK_DIST_C_FILES)

all: print opt

opt: $(CLIBDIR) $(COBJDIR) $(CBINDIR) $(BIN_TARGETS) 

test: $(TESTDIR) $(TEST_BIN_FILES) #test_suite

print:
#	echo "NOLINK_DIST_C_MODULES = ${NOLINK_DIST_C_MODULES}"
#	echo "NOLINK_DIST_C_FILES = ${NOLINK_DIST_C_FILES}"
#	echo "NOLINK_DIST_OBJ_FILES = ${NOLINK_DIST_OBJ_FILES}"
#	echo "NOLINK_DIST_BIN_FILES =${NOLINK_DIST_BIN_FILES}"

$(COBJDIR):
	$(INSTALL_DIR) $(COBJDIR)

$(CBINDIR):
	$(INSTALL_DIR) $(CBINDIR)

$(CLIBDIR):
	$(INSTALL_DIR) $(CLIBDIR)

$(TESTDIR):
	$(INSTALL_DIR) $(TESTDIR)

clean:
	$(RM) $(BIN_TARGETS) $(DIST_OBJ_FILES) \
	$(NOLINK_DIST_OBJ_FILES) $(ERLANG_LINK_DIST_OBJ_FILES) \
	$(TEST_BIN_FILES) 

# ----------------------------------------------------
# Special Build Targets
# ----------------------------------------------------

$(DIST_BIN_FILES): $(DIST_C_FILES)  
	$(CC) $(TYPE_FLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $<

# Test cases compilation
$(TEST_OBJ_FILES): $(TEST_C_FILES) 
	$(CC) -c $(TYPE_FLAGS) $(LDFLAGS) $(ERLANG_LDLIBS) -o $@ $(notdir $(basename $@))_test.c

$(TEST_BIN_FILES): $(TEST_OBJ_FILES) $(TEST_ENV_OBJ_FILES) 
	$(CC) $(TYPE_FLAGS) $(LDFLAGS) $(ERLANG_LDLIBS) $? $(COBJDIR)/$(notdir $(basename $@)).o -lpthread -ldl -o $@ 

# $(*F) means elements (one at a time) from targets without path and suffix
# $@ means elements (one at a time) from targets
$(NOLINK_DIST_OBJ_FILES): $(NOLINK_DIST_C_FILES)
	$(CC) -c $(TYPE_FLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $(*F).c


$(CBINDIR)/throw_test: $(NOLINK_DIST_OBJ_FILES) throw_test.c
	$(CC) -fPIC $(TYPE_FLAGS) $(LDFLAGS) -lpthread -ldl  $^ -o $@ 
	echo "The result is in $@"

$(CBINDIR)/sum.test: $(NOLINK_DIST_OBJ_FILES) $(ERLANG_LINK_DIST_OBJ_FILES)
	$(CC) -fPIC $(TYPE_FLAGS) $(LDFLAGS) -lpthread -ldl  -o $@ 
	echo "The result is in $@"

# ----------------------------------------------------
# Release Target
# ---------------------------------------------------- 
release_spec: opt
	$(INSTALL_DATA) $(BIN_TARGETS) $(SERVICE_BIN)