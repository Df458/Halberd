class HalberdEditor : Gtk.Application
{
    StartupWindow startup_win;
    MainWindow window;
    NewProjectDialog new_dialog;
    public Gtk.Window? current_win = null;
    public Settings settings;
    public bool failed_init = false;

    File main_directory;
    File project_directory;

    public HalberdEditor(string[] args)
    {
        settings = new GLib.Settings("org.df458.halberd");
        if(!init_main_directory()) {
            failed_init = true;
        }
        Halberd.Game.Settings.init();
        Halberd.Editor.Cursor.init();
    }

    public File get_content_directory()
    {
        return File.new_for_path(Halberd.IO.get_path());
    }

    public string? path_to_content_path(string path)
    {
        string cpath = Halberd.IO.get_path();

        if(path.substring(0, cpath.length) == cpath) {
            if(path.length <= cpath.length + 1)
                return null;
            return path.substring(cpath.length + 1, path.length - cpath.length - 1);
        }

        return path;
    }

    public int runall()
    {
        string? last_path;
        bool? open_last;
        last_path = settings.get_string("last-project");
        open_last = settings.get_boolean("open-last");
        if(open_last && last_path != null && last_path.length > 0) {
            load_project(last_path);
        } else {
            if(open_last == null)
                open_last = false;

            startup_win = new StartupWindow(main_directory, open_last);
            current_win = startup_win;
        }

        if(!failed_init)
            Gtk.main();
        Halberd.Game.Settings.cleanup();
        return 0;
    }

    public void new_project()
    {
        new_dialog = new NewProjectDialog(current_win, main_directory.get_path());
        new_dialog.response_selected.connect((cr, pa) => {
            if(cr == true && pa != null && Halberd.Editor.create_project(pa)) {
                load_project(pa, true);
            }
            new_dialog.destroy();
        });
        new_dialog.display();
    }

    public void load_dialog()
    {
        Gtk.FileChooserDialog fc = new Gtk.FileChooserDialog("Load a project", current_win, Gtk.FileChooserAction.OPEN, "Cancel", Gtk.ResponseType.CANCEL, "Open", Gtk.ResponseType.ACCEPT);
        Gtk.FileFilter filter = new Gtk.FileFilter();
        filter.add_pattern("*.hal");
        filter.set_filter_name("Halberd Project Files");
        fc.add_filter(filter);
        fc.response.connect((r) => {
            if(r == Gtk.ResponseType.ACCEPT) {
                string path = fc.get_file().get_path();
                load_project(path);
            }
            fc.destroy();
        });
        fc.run();
    }

    public void load_project(string project_path, bool from_folder=false)
    {
        project_directory = File.new_for_path(project_path);
        string full_path = project_path;
        if(from_folder) {
            full_path += "/" + project_directory.get_basename() + ".hal";
        } else {
            project_directory = project_directory.get_parent();
        }
        settings.set_string("last-project", full_path);
        Gtk.RecentManager.get_default().add_item("file://" + full_path);
        Halberd.Editor.load_project(full_path);
        // TODO: Test version and prompt for upgrade
        if(window == null)
            window = new MainWindow();
        else
            window.reload();
        current_win = window;
        if(startup_win != null)
            startup_win.destroy();
    }

    public void window_destroyed(Gtk.Window win)
    {
        if(win == current_win) {
            Gtk.main_quit();
        }
    }

    private bool init_main_directory()
    {
        try {
            string? path;
            path = settings.get_string("project-dir");
            if(path == null || path.length == 0) {
                stderr.printf("No main directory set. Generating one now...\n");
                path = Environment.get_user_data_dir() + "/Halberd";
                settings.set_string("project-dir", path);
            }
            main_directory = File.new_for_path(path);
            if(!main_directory.query_exists())
                main_directory.make_directory();
        } catch(Error err) {
            display_warning("Error initializing the project directory: " + err.message + "\n");
            return false;
        }
        return true;
    }

    /*
     * This function displays a warning in the terminal, and also creates a message
     * dialog that displays it to the end-user.
     */
    public void display_warning(string format)
    {
        warning(format);
        Gtk.MessageDialog dialog = new Gtk.MessageDialog(current_win, Gtk.DialogFlags.MODAL, Gtk.MessageType.WARNING, Gtk.ButtonsType.OK, format);
        dialog.response.connect((r) => { dialog.destroy(); });
        dialog.show();
    }

    public void display_about()
    {
        Gtk.AboutDialog about_dialog = new Gtk.AboutDialog();
        about_dialog.set_authors({"Hugues Ross"});
        about_dialog.set_program_name("Halberd RPG Engine");
        about_dialog.set_website("github.com/Df458/Halberd");
        about_dialog.response.connect(() => { about_dialog.close(); });
        about_dialog.set_transient_for(current_win);
        about_dialog.run();
    }
}

/*
 * This function takes a resource extension and filename and creates a GIO File
 * from the path returned by construct_extended_resource_path.
 */
public File file_from_resource(string? ext, string name, bool new_file = false)
{
    string path = Halberd.IO.make_path(ext, name);
    if(new_file)
        path = Halberd.IO.make_path(ext, Halberd.IO.get_unique_name(ext, name));
    File f = File.new_for_path(path);
    return f;
}

/*
 * Returns whether or not a file is part of the current project's content
 */
public bool file_is_content(File file)
{
    string? path = file.get_path();
    string cpath = Halberd.IO.get_path();

    if(path != null && path.length > cpath.length) {
        if(path.substring(0, cpath.length) == cpath) {
            return true;
        }
    }

    g_free(path);
    return false;
}
