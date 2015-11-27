class HalberdEditor : Gtk.Application
{
    StartupWindow startup_win;
    MainWindow window;
    NewProjectDialog new_dialog;
    Gtk.Window? current_win = null;
    public Settings settings;

    File main_directory;
    File project_directory;

    public HalberdEditor(string[] args)
    {
        settings = new GLib.Settings("org.df458.halberd");
        Halberd.Game.Settings.init();
        Halberd.Editor.Cursor.init();
        if(!init_main_directory()) {
            // TODO: End the program
        }

        string? last_path;
        last_path = settings.get_string("last-project");
        if(settings.get_boolean("open-last") && last_path != null) {
            // TODO: Check to make sure there is a last folder, yada yada
            load_project(last_path);
        } else {
            startup_win = new StartupWindow(main_directory);
            current_win = startup_win;
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

    public int runall()
    {
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
}

public void display_warning(string format, ...)
{
    warning(format);
    // TODO: Display a popup with the warning
}
