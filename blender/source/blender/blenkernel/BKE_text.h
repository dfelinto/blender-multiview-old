/**
 * blenlib/BKE_text.h (mar-2001 nzc)
 *	
 * $Id$ 
 *
 * ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
 *
 * The contents of this file may be used under the terms of either the GNU
 * General Public License Version 2 or later (the "GPL", see
 * http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
 * later (the "BL", see http://www.blender.org/BL/ ) which has to be
 * bought from the Blender Foundation to become active, in which case the
 * above mentioned GPL option does not apply.
 *
 * The Original Code is Copyright (C) 2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL/BL DUAL LICENSE BLOCK *****
 */
#ifndef BKE_TEXT_H
#define BKE_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

struct Text;
struct TextLine;

void			free_text		(struct Text *text);
void 			txt_set_undostate	(int u);
int 			txt_get_undostate	(void);
struct Text*	add_empty_text	(void);
int	            reopen_text		(struct Text *text);
struct Text*	add_text		(char *file); 
struct Text*	copy_text		(struct Text *ta);

void	txt_free_cut_buffer	(void);

char*	txt_to_buf			(struct Text *text);
void	txt_clean_text		(struct Text *text);
void	txt_order_cursors	(struct Text *text);
int		txt_find_string		(struct Text *text, char *findstr);
int		txt_has_sel			(struct Text *text);
int		txt_get_span		(struct TextLine *from, struct TextLine *to);
void	txt_move_up			(struct Text *text, short sel);
void	txt_move_down		(struct Text *text, short sel);
void	txt_move_left		(struct Text *text, short sel);
void	txt_move_right		(struct Text *text, short sel);
void	txt_move_bof		(struct Text *text, short sel);
void	txt_move_eof		(struct Text *text, short sel);
void	txt_move_bol		(struct Text *text, short sel);
void	txt_move_eol		(struct Text *text, short sel);
void	txt_move_toline		(struct Text *text, unsigned int line, short sel);
void	txt_pop_sel			(struct Text *text);
void	txt_delete_char		(struct Text *text);
void	txt_copy_sel		(struct Text *text);
void	txt_sel_all			(struct Text *text);
void	txt_sel_line		(struct Text *text);
void	txt_print_cutbuffer	(void);
void	txt_cut_sel			(struct Text *text);
char*	txt_sel_to_buf		(struct Text *text);
void	txt_insert_buf		(struct Text *text, char *in_buffer);
void	txt_paste			(struct Text *text);
void	txt_print_undo		(struct Text *text);
void	txt_undo_add_toop	(struct Text *text, int op, unsigned int froml, unsigned short fromc, unsigned int tol, unsigned short toc);
void	txt_do_undo			(struct Text *text);
void	txt_do_redo			(struct Text *text);
void	txt_split_curline	(struct Text *text);
void	txt_backspace_char	(struct Text *text);
int		txt_add_char		(struct Text *text, char add);


/* Undo opcodes */

/* Simple main cursor movement */
#define UNDO_CLEFT		001
#define UNDO_CRIGHT		002
#define UNDO_CUP		003
#define UNDO_CDOWN		004

/* Simple selection cursor movement */
#define UNDO_SLEFT		005
#define UNDO_SRIGHT		006
#define UNDO_SUP		007
#define UNDO_SDOWN		021

/* Complex movement (opcode is followed
 * by 4 character line ID + a 2 character
 * position ID and opcode (repeat)) */
#define UNDO_CTO		022
#define UNDO_STO		023

/* Complex editing (opcode is followed
 * by 1 character ID and opcode (repeat)) */
#define UNDO_INSERT		024
#define UNDO_BS			025
#define UNDO_DEL		026

/* Text block (opcode is followed
 * by 4 character length ID + the text
 * block itself + the 4 character length
 * ID (repeat) and opcode (repeat)) */
#define UNDO_DBLOCK		027 /* Delete block */
#define UNDO_IBLOCK		030 /* Insert block */

/* Misc */
#define UNDO_SWAP		031	/* Swap cursors */

#ifdef __cplusplus
}
#endif

#endif
