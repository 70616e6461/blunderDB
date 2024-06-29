#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iup.h>
#include <iupdraw.h>
#include <cd.h>
#include <cdiup.h>
#include <sqlite3.h>

char db_file[10240];
cdCanvas *winCanvas = NULL;
cdCanvas *curCanvas = NULL;

/************************ Database ***********************/
sqlite3 *db = NULL;
bool is_db_saved = true;
int rc;
char *errMsg = 0;

const char *sql_collection =
"CREATE TABLE collection ("
"id INTEGER PRIMARY KEY AUTOINCREMENT,"
"name TEXT,"
"position_list_id INTEGR,"
"FOREIGN KEY(position_list_id) REFERENCES position_list(id)"
");";

const char *sql_position_list =
"CREATE TABLE position_list ("
"id INTEGER PRIMARY KEY AUTOINCREMENT,"
"position_id INTEGER,"
"FOREIGN KEY(position_id) REFERENCES position(id)"
");";

const char *sql_player = 
"CREATE TABLE player ("
"id INTEGER PRIMARY KEY AUTOINCREMENT,"
"name TEXT"
");";


const char *sql_position =
"CREATE TABLE position ("
"id INTEGER PRIMARY KEY AUTOINCREMENT,"
"p0 INTEGER,"
"p1 INTEGER,"
"p2 INTEGER,"
"p3 INTEGER,"
"p4 INTEGER,"
"p5 INTEGER,"
"p6 INTEGER,"
"p7 INTEGER,"
"p8 INTEGER,"
"p9 INTEGER,"
"p10 INTEGER,"
"p11 INTEGER,"
"p12 INTEGER,"
"p13 INTEGER,"
"p14 INTEGER,"
"p15 INTEGER,"
"p16 INTEGER,"
"p17 INTEGER,"
"p18 INTEGER,"
"p19 INTEGER,"
"p20 INTEGER,"
"p21 INTEGER,"
"p22 INTEGER,"
"p23 INTEGER,"
"p24 INTEGER,"
"p25 INTEGER,"
"player1_id INTEGER,"
"player2_id INTEGER,"
"player1_score INTEGER,"
"player2_score INTEGER,"
"cube_position INTEGER,"
"comment TEXT,"
"FOREIGN KEY(player1_id) REFERENCES player(id),"
"FOREIGN KEY(player2_id) REFERENCES player(id)"
");";

;

void execute_sql(sqlite3 *db, const char *sql)
{
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if(rc != SQLITE_OK) {
        printf("SQL error: %s\n", errMsg);
    } else {
        printf("SQL executed successfully\n");
    }
}

int db_create(const char* filename)
{
    if (remove(filename) == 0) {
        printf("Existing database file removed successfully\n");
    } else {
        printf("No existing database file to remove, or failed to remove\n");
    }

    rc = sqlite3_open(filename, &db);
    printf("%s\n", sql_position);

    if(rc) {
        printf("Can't create database: %s\n", sqlite3_errmsg(db));
        return rc;
    } else {
        printf("Created database successfully\n");
    }

    printf("Try to create player table.\n");
    execute_sql(db, sql_player);

    printf("Try to create position table.\n");
    execute_sql(db, sql_position);

    printf("Try to create position_list table.\n");
    execute_sql(db, sql_position_list);

    printf("Try to create collection table.\n");
    execute_sql(db, sql_collection);

    return 0;
}

int db_open(const char* filename)
{
    rc = sqlite3_open(filename, &db);

    if(rc) {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        return rc;
    } else {
        printf("Opened database successfully\n");
    }

    return 0;

}

int db_close(sqlite3 *db)
{
    rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        printf("Can't close database. Maybe already closed. Err: %s\n", sqlite3_errmsg(db));
    } else {
        printf("Closed database successfully\n");
    }

}

/************************ Interface ***********************/

#define DEFAULT_SIZE "800x600"

Ihandle *dlg, *hbox, *vbox, *label;
Ihandle *text;
Ihandle *menu, *submenu_file, *submenu_edit,
        *submenu_position, *submenu_match,
        *submenu_search, *submenu_tool,
        *submenu_help;

Ihandle *menu_file;
Ihandle *item_new, *item_open, *item_recent, *item_exit;
Ihandle *item_import, *item_import_wizard;
Ihandle *item_save, *item_saveas;
Ihandle *item_export;
Ihandle *item_properties;
Ihandle *item_exit;

Ihandle *menu_edit;
Ihandle *item_undo, *item_redo, *item_copy, *item_cut, *item_paste;
Ihandle *item_editmode;

Ihandle *menu_position;
Ihandle *item_next_position, *item_prev_position,
        *item_new_position,
        *item_import_position, *item_import_position_bybatch;
Ihandle *item_new_collection;
Ihandle *item_delete_collection;
Ihandle *item_add_collection;

Ihandle *menu_match;
Ihandle *item_import_match, *item_import_match_bybatch, 
        *item_match_library;

Ihandle *menu_search;
Ihandle *item_search_blunder, *item_search_structure,
        *item_search_dice, *item_search_cube, *item_search_score,
        *item_search_player, *item_search_engine;
Ihandle *item_searchmode;

Ihandle *menu_tool;
Ihandle *item_find_position_without_analysis;
Ihandle *item_preferences;

Ihandle *menu_help;
Ihandle *item_manual, *item_userguide, *item_tips, *item_cmdmode;
Ihandle *item_keyboard;
Ihandle *item_getinvolved, *item_donate;
Ihandle *item_about;

Ihandle *canvas;

static int canvas_action_cb(Ihandle* ih);
static int item_new_action_cb(void);
static int item_open_action_cb(void);
static int item_save_action_cb(void);
static int item_exit_action_cb();
void error_callback(void);



static int canvas_action_cb(Ihandle* ih)
{
    int i, w, h;
    cdCanvas *canvas;

    canvas = cdCreateCanvas(CD_IUP, ih);
    cdCanvasGetSize(canvas, &w, &h, NULL, NULL);

    cdCanvasBackground(canvas, CD_BLUE);
    cdCanvasClear(canvas);

    cdCanvasLineWidth(canvas, 3);
    cdCanvasLineStyle(canvas, CD_CONTINUOUS);
    cdCanvasForeground(canvas, cdEncodeAlpha(CD_DARK_MAGENTA, 128));
    cdCanvasRect(canvas, 100, 200, 100, 200);

    cdCanvasSetAttribute(canvas, "DRAWCOLOR", "252 186 3");

    cdCanvasFlush(canvas);

    return IUP_DEFAULT;
}

static int item_new_action_cb(void)
{

    Ihandle *filedlg;

    filedlg = IupFileDlg();
    IupSetAttribute(filedlg, "DIALOGTYPE", "SAVE");
    IupSetAttribute(filedlg, "TITLE", "New Database");
    IupSetAttribute(filedlg, "EXTFILTER",
            "Blunder Database (.db)|*.db|All Files|*.*|");
    IupSetAttribute(filedlg, "EXTDEFAULT", ".db");
    IupPopup(filedlg, IUP_CENTER, IUP_CENTER);
    
    switch(IupGetInt(filedlg, "STATUS"))
    {
        case 1: // new file
        case 0 : // file already exists
            const char *db_filename = IupGetAttribute(filedlg, "VALUE");
            int result = db_create(db_filename);
            if (result != 0) {
                printf("Database creation failed\n");
                return result;
            }
            printf("Database created successfully\n");
            break; 

        case -1 : 
            printf("IupFileDlg","Operation Canceled");
            return 1;
            break; 
    }

    IupDestroy(filedlg);
    return IUP_DEFAULT;
}


static int item_open_action_cb(void)
{
    Ihandle *filedlg;

    filedlg = IupFileDlg();
    IupSetAttribute(filedlg, "DIALOGTYPE", "OPEN");
    IupSetAttribute(filedlg, "TITLE", "Open Database");
    IupSetAttribute(filedlg, "EXTFILTER",
            "Blunder Database (.db)|*.db|All Files|*.*|");
    IupSetAttribute(filedlg, "EXTDEFAULT", ".db");
    IupPopup(filedlg, IUP_CENTER, IUP_CENTER);
    
    switch(IupGetInt(filedlg, "STATUS"))
    {
        case 1: // new file
            printf("Database does not exist.");
            break;
        case 0 : // file already exists
            const char *db_filename = IupGetAttribute(filedlg, "VALUE");
            int result = db_open(db_filename);
            if (result != 0) {
                printf("Database opening failed\n");
                return result;
            }
            printf("Database opened successfully\n");
            break; 

        case -1 : 
            printf("IupFileDlg","Operation Canceled");
            return 1;
            break; 
    }

    IupDestroy(filedlg);
    return IUP_DEFAULT;

}

static int item_save_action_cb(void)
{
    error_callback();
}


static int item_exit_action_cb()
{
    // verify if db is saved with is_db_saved before quitting.

    db_close(db);
    IupClose();
    return EXIT_SUCCESS;
}

void error_callback(void)
{
    IupMessage("Callback Error", "Functionality not implemented yet!");
}

/************************ Main ****************************/
int main(int argc, char **argv)
{
  IupOpen(&argc, &argv);
  IupSetLanguage("ENGLISH");

  /* item_recent = IupItem("Recent", NULL); */
  /* item_import = IupItem("Import...", NULL); */
  /* item_import_wizard = IupItem("Import Wizard", NULL); */
  /* item_export = IupItem("Export...", NULL); */
  /* item_properties = IupItem("Properties", NULL); */
  /* item_exit = IupItem("Exit", "item_exit_act"); */
  /* menu_file = IupMenu(item_new, item_open, item_recent, item_exit, */
  /*         IupSeparator(), item_save, item_saveas, */ 
  /*         IupSeparator(), item_import, item_import_wizard, */ 
  /*         IupSeparator(), item_export, */ 
  /*         IupSeparator(), item_properties, */
  /*         IupSeparator, item_exit, NULL); */

  /* item_undo = IupItem("Undo", NULL); */
  /* item_redo = IupItem("Redo", NULL); */
  /* item_copy = IupItem("Copy", NULL); */
  /* item_cut = IupItem("Cut", NULL); */
  /* item_paste = IupItem("Paste", NULL); */
  /* item_editmode = IupItem("Edit Mode", NULL); */
  /* menu_edit = IupMenu(item_undo, item_redo, */
  /*         item_copy, item_cut, item_paste, */
  /*         IupSeparator(), item_editmode, NULL); */

  /* item_next_position = IupItem("Next Position", NULL); */
  /* item_prev_position = IupItem("Previous Position", NULL); */
  /* item_new_position = IupItem("New Position", NULL); */
  /* item_import_position = IupItem("Import", NULL); */
  /* item_import_position_bybatch = IupItem("Import by Batch", NULL); */
  /* item_new_collection = IupItem("New Library", NULL); */
  /* item_delete_collection = IupItem("Delete Library", NULL); */
  /* item_add_collection = IupItem("Add to Library", NULL); */
  /* menu_position = IupMenu(item_next_position, item_prev_position, */ 
  /*         item_new_position, item_import_position, */
  /*         item_import_position_bybatch, IupSeparator(), */
  /*         item_new_collection, item_delete_collection, */
  /*         item_add_collection, NULL); */

  /* item_import_match = IupItem("Import Match", NULL); */
  /* item_import_match_bybatch = IupItem("Import Matches by Batch", */
  /*         NULL); */
  /* item_match_library = IupItem("Match Library", NULL); */
  /* menu_match = IupMenu(item_import_match, item_import_match_bybatch, */
  /*         item_match_library, NULL); */

  /* item_search_blunder = IupItem("by Blunder", NULL); */
  /* item_search_structure = IupItem("by Dice", NULL); */
  /* item_search_cube = IupItem("by Cube Decision", NULL); */
  /* item_search_score = IupItem("by Score", NULL); */
  /* item_search_player = IupItem("by Player", NULL); */
  /* item_search_engine = IupItem("Search Engine", NULL); */
  /* item_searchmode = IupItem("Search Mode", NULL); */
  /* menu_search = IupMenu(item_search_blunder, */
  /*         item_search_structure, item_search_cube, */
  /*         item_search_score, item_search_player, */
  /*         item_search_engine, IupSeparator(), */
  /*         item_searchmode, NULL); */

  /* item_find_position_without_analysis = IupItem("Find Positions without Analysis", NULL); */
  /* item_preferences = IupItem("Preferences", NULL); */
  /* menu_tool = IupMenu(item_find_position_without_analysis, */
  /*         IupSeparator(), item_preferences, NULL); */

  /* item_manual = IupItem("Help Manual", NULL); */
  /* item_userguide = IupItem("User Guide", NULL); */
  /* item_tips = IupItem("Tips", NULL); */
  /* item_cmdmode = IupItem("Command Mode Help", NULL); */
  /* item_keyboard = IupItem("Keyboard shortcuts", NULL); */
  /* item_getinvolved = IupItem("Get Involved", NULL); */
  /* item_donate = IupItem("Donate to blunderDB", NULL); */
  /* item_about = IupItem("About", NULL); */
  /* menu_help = IupMenu(item_manual, item_userguide, */
  /*         item_tips, item_cmdmode, item_keyboard, */
  /*         IupSeparator(), item_getinvolved, item_donate, */
  /*         IupSeparator(), item_about, NULL); */

  /* submenu_file = IupSubmenu("File", menu_file); */
  /* submenu_edit = IupSubmenu("Edit", menu_edit); */
  /* submenu_position = IupSubmenu("Positions", menu_position); */
  /* submenu_match = IupSubmenu("Matches", menu_match); */
  /* submenu_search = IupSubmenu("Search", menu_search); */
  /* submenu_help = IupSubmenu("Help", menu_help); */

  /* menu = IupMenu(submenu_file, submenu_edit, submenu_position, */
  /*         submenu_match, submenu_search, submenu_tool, submenu_help, */
  /*         NULL); */

  /* Define menus */
  item_new = IupItem("&New\tCtrl+N", NULL);
  item_open = IupItem("&Open\tCtrl+O", NULL);
  item_save = IupItem("&Save", NULL);
  /* item_saveas = IupItem("Save &As...", NULL); */
  item_exit = IupItem("E&xit\tCtrl+Q", NULL);
  menu_file = IupMenu(item_new, item_open,
          IupSeparator(), item_save,
          IupSeparator(), item_exit, NULL);
  submenu_file = IupSubmenu("&File", menu_file);
  menu = IupMenu(submenu_file, NULL);
  IupSetHandle("menu", menu);

  /* Define main canvas */
  canvas = IupCanvas(NULL);
  IupSetAttribute(canvas, "NAME", "CANVAS");
  IupSetAttribute(canvas, "EXPAND", "YES");

  /* General layout */
  vbox = IupVbox(canvas, NULL);
  IupSetAttribute(vbox, "NMARGIN", "10x10");
  IupSetAttribute(vbox, "GAP", "10");

  /* Main Windows */
  dlg = IupDialog(vbox);
  IupSetAttribute(dlg, "TITLE", "blunderDB");
  IupSetAttribute(dlg, "SIZE", DEFAULT_SIZE);
  IupSetAttribute(dlg, "MENU", "menu");

  /* Registers callbacks */
  IupSetCallback(dlg, "K_cN", (Icallback) item_new_action_cb);
  IupSetCallback(dlg, "K_cO", (Icallback) item_open_action_cb);
  IupSetCallback(dlg, "K_cS", (Icallback) item_save_action_cb);
  IupSetCallback(dlg, "K_cQ", (Icallback) item_exit_action_cb);
  IupSetCallback(item_new, "ACTION", (Icallback) item_new_action_cb);
  IupSetCallback(item_open, "ACTION", (Icallback) item_open_action_cb);
  IupSetCallback(item_save, "ACTION", (Icallback) item_save_action_cb);
  IupSetCallback(item_exit, "ACTION", (Icallback) item_exit_action_cb);
  IupSetCallback(canvas, "ACTION", (Icallback)canvas_action_cb);


  IupShowXY(dlg, IUP_CENTER, IUP_CENTER);

  IupMainLoop();

  db_close(db);
  IupClose();
  return EXIT_SUCCESS;
}
