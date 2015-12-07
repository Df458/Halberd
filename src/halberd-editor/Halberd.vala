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
        Halberd.Game.Settings.init();
        Halberd.Editor.Cursor.init();
        if(!init_main_directory()) {
            failed_init = true;
        }

        string? last_path;
        last_path = settings.get_string("last-project");
        if(settings.get_boolean("open-last") && last_path != null) {
            load_project(last_path);
        } else {
            startup_win = new StartupWindow(main_directory);
            current_win = startup_win;
        }
    }

    public File get_content_directory()
    {
        return File.new_for_path(Halberd.get_resource_path());
    }

    public int runall()
    {
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
        Gtk.RecentManager.get_default().add_item("file://" + full_path);
        Halberd.Editor.load_project(full_path);
        // TODO: Test version and prompt for upgrade
        if(window == null)
            window = new MainWindow();
        else
            window.reload();
        current_win = window;
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
}

/*
 * This function ensures that a new file has a name that isn't taken. If it is,
 * then the file has a number appended to it, starting from 1.
 */
public void ensure_new(File infile)
{
    string base_name = infile.get_basename();
    uint i = 1;
    while(infile.query_exists()) {
        try {
            infile.set_display_name(base_name + i.to_string());
            ++i;
        } catch(Error e) {
            warning("Error renaming file: %s\n", e.message);
        }
    }
}
