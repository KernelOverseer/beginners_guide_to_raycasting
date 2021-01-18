# _lCYAN = \x1b[96m
# _lYELLOW = \x1b[93m
# _lGREEN = \x1b[92m
# _lBLUE = \x1b[94m
# _RED = \x1b[31m
# _BOLD = \x1b[1m
# _END = \x1b[0m
CC = gcc
NAME = cub

FILENAMES = main.c\
			raycast.c\
			debug.c

HEADER_FILE = cub.h

SRC_DIR = ./src
OBJ_DIR = ./obj
INC_DIR = ./inc

OBJ = $(addprefix $(OBJ_DIR)/, $(FILENAMES:.c=.o))

HEADER_FILES = $(addprefix $(INC_DIR)/, $(HEADER_FILE))

FLAGS = #-g -Wall -Werror -Wextra

FTMATHS = ./libs/ft_maths

LIBFTMATHS = $(FTMATHS)/libftmaths.a

LIBFTDIR = ./libs/libft

LIBFT = $(LIBFTDIR)/libft.a

LIBTTSLISTDIR = ./libs/ttslist

LIBTTSLIST = $(LIBTTSLISTDIR)/libttslist.a

INCLUDES = -I ./libs/FT_SimpleSDL/inc -D_REENTRANT -I/usr/include/SDL2 -lSDL2 -I ./inc -I $(LIBFTDIR) -I ./libs/ft_maths/inc -I $(LIBTTSLISTDIR)/includes

# detecting the os and linking with the good minilibx

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	LIBRARIES = -L ./libs/FT_SimpleSDL -lftsimplesdl -L $(LIBFTDIR) -lft -lpthread -L $(FTMATHS) -lftmaths -lm -L $(LIBTTSLISTDIR) -lttslist
else
	LIBRARIES = -L $(LIBFTDIR) -lft -L $(FTMATHS) -lftmaths -L $(LIBTTSLISTDIR) -lttslist
endif

DELAY = 0

all : $(NAME)
$(NAME): $(MAIN_OBJ) $(OBJ) $(LIBFT) $(LIBFTMATHS) $(LIBTTSLIST)
	@echo "$(CC) $(_lYELLOW)$(FLAGS)$(_END) $(_lCYAN)$(OBJ)$(_END)\n$(_lGREEN)$(LIBRARIES)$(_END) -I$(_RED)$(INC_DIR)$(_END)$(_RED)$(INCLUDES)$(_END) -o $(_lBLUE)$(NAME)$(_lEND)$(_RED)\n"
	@$(CC) $(FLAGS) $(MAIN_OBJ) $(OBJ) $(LIBRARIES) $(INCLUDES) -o $(NAME)

$(SLAVE_NAME): $(SLAVE_OBJ) $(OBJ) $(LIBFT) $(LIBFTMATHS) $(LIBTTSLIST)
	@echo "$(CC) $(_lYELLOW)$(FLAGS)$(_END) $(_lCYAN)$(OBJ)$(_END)\n$(_lGREEN)$(LIBRARIES)$(_END) -I$(_RED)$(INC_DIR)$(_END)$(_RED)$(INCLUDES)$(_END) -o $(_lBLUE)$(NAME)$(_lEND)$(_RED)\n"
	@$(CC) $(FLAGS) $(SLAVE_OBJ) $(OBJ) $(LIBRARIES) $(INCLUDES) -o $(SLAVE_NAME)

$(OBJ): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(HEADER_FILES) | $(OBJ_DIR)
	$(CC) $(FLAGS) -c $< -o $@ -I $(INC_DIR) $(INCLUDES)

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(LIBFTMATHS):
	@make -C $(FTMATHS)

$(LIBFT):
	@make -C $(LIBFTDIR)

$(LIBTTSLIST):
	@make -C $(LIBTTSLISTDIR)

clean:
	@echo "\n$(_lCYAN)Makefile :$(_END) will delete $(_RED)$(OBJ) $(VIS_OBJ)$(_END)"
	@echo "starting in $(DELAY) sec, $(_RED)press Ctrl-c to abort$(_END)"
	@sleep $(DELAY)
	@make -C $(LIBFTDIR) clean
	@make -C $(FTMATHS) clean
	@make -C $(LIBTTSLISTDIR) clean
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "\n$(_lCYAN)Makefile :$(_END) will delete $(_RED)$(NAME)$(_END)"
	@echo "starting in $(DELAY) sec, $(_RED)press Ctrl-c to abort$(_END)"
	@sleep $(DELAY)
	@make -C $(LIBFTDIR) fclean
	@make -C $(FTMATHS) fclean
	@make -C $(LIBTTSLISTDIR) fclean
	@rm -f $(NAME)
re: fclean all
