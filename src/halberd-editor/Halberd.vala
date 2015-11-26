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
            startup_win = new StartupWindow();
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
            if(cr == true && pa != null & Halberd.Editor.create_project(pa)) {
                load_project(pa);
            }
        });
        new_dialog.display();
    }

    public void load_project(string project_path)
    {
        project_directory = File.new_for_path(project_path);
        window = new MainWindow();
    }


    //private bool create_project(string path)
    //{
        ////project_directory = File.new_for_path(path);
        ////if(!project_directory.query_exists()) {
            ////try {
                ////project_directory.make_directory();
            ////} catch(Error err) {
                ////display_warning("Error creating a new project: " + err.message + "\n");
                ////return false;
            ////}
        ////} else {
            ////display_warning("Error creating a new project: Project directory already exists\n");
            ////return false;
        ////}
        //bool success = Halberd.Editor.create_project(path);

        //// TODO: Init additional project dirs and files
        //return true;
    //}
}

public void display_warning(string format, ...)
{
    warning(format);
    // TODO: Display a popup with the warning
}
