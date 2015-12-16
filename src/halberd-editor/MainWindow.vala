using Gtk;
using Halberd;

public class MainWindow : Window
{
    public AccelGroup accel;
    public int icon_size = 5;

    // FIle Icons
    // TODO: Replace with string icon names
    Gdk.Pixbuf default_icon;
    Gdk.Pixbuf folder_icon;
    //Gdk.Pixbuf map_icon;
    //Gdk.Pixbuf image_icon;

    private HeaderBar topbar;

    private EditorEmbed editor;
    private GameEmbed game;
    private EmbeddableView current_embed;
    private GLArea viewport;

    // Top Buttons
    private Button button_save;
    private ToggleButton button_play;
    private MenuButton button_menu;

    // Toolbar
    private Separator tool_separator;
    private Toolbar toolbar;
    private ToolButton button_draw;
    private ToolButton button_fill;
    private ToolButton button_erase;

    // Project Pane
    private Button import_button;
    private SearchEntry filter_entry;
    private TreeView project_tree_view;
    private IconView project_icon_view;
    private TreeStore project_tree_data;
    private TreeModelFilter project_content_data;
    private TreeModelFilter project_tree_filter;
    private Overlay project_content_overlay;
    private Scale icon_scale;
    private Gtk.Menu content_menu;
    private Gtk.Menu content_file_menu;
    FileMonitor? content_monitor = null;

    // Containers
    private Paned main_paned;
    private Paned files_paned;
    private Box   files_box;
    private Box   files_control_box;
    private Box   toolbar_box;
    private Gtk.Stack view_stack;

    string current_map_name = "";
    string? current_map_path = null;

    public MainWindow()
    {
        init_structure();
        init_content();
        connect_signals();
        add_actions();
        init_menus();

        add_project_directory(app.get_content_directory(), null);
        TreePath path = new TreePath.from_indices(0);
        project_tree_view.expand_to_path(path);
        project_tree_view.set_cursor(path, null, false);

        this.show_all();
    }

    public void reload()
    {
        topbar.subtitle = Editor.get_loaded_project_name();
        // TODO: Reload the editor here
    }

    /*!
     * This function returns the current sub-path of the resource directory
     * that's open in the project pane.
     */
    public string? get_selected_path()
    {
        string path_string = "";
        TreePath path;

        // We use the icon view and convert, because the tree view might be
        // deselected
        path = project_content_data.virtual_root;

        // If nothing is selected, default to the base path
        if(path == null || path.get_depth() == 1)
            return null;

        while(path.get_depth() > 1) {
            string temp_name;
            TreeIter iter;
            project_tree_data.get_iter(out iter, path);
            project_tree_data.get(iter, 0, out temp_name);
            if(path_string != "")
                path_string = temp_name + "/" + path_string;
            else
                path_string = temp_name;
            path.up();
        }

        return path_string;
    }

    private void init_structure()
    {
        topbar = new HeaderBar();
        main_paned = new Paned(Orientation.VERTICAL);
        files_paned = new Paned(Orientation.HORIZONTAL);
        files_box = new Box(Orientation.VERTICAL, 6);
        files_control_box = new Box(Orientation.HORIZONTAL, 6);
        toolbar_box = new Box(Orientation.HORIZONTAL, 0);
        tool_separator = new Separator(Orientation.VERTICAL);
        toolbar = new Toolbar();
        view_stack = new Gtk.Stack();
        project_content_overlay = new Overlay();

        this.window_position = WindowPosition.CENTER;
        this.set_default_size(1024, 768);
        accel = new Gtk.AccelGroup();
        this.add_accel_group(accel);
        topbar.title = "Halberd RPG Engine";
        topbar.subtitle = Editor.get_loaded_project_name();
        topbar.show_close_button = true;
        topbar.decoration_layout = "menu:close";
        files_box.margin_top = 6;
        files_box.set_homogeneous(false);
        files_control_box.margin_start = 12;
        files_control_box.margin_end = 12;
        toolbar.orientation_changed(Orientation.VERTICAL);
        toolbar.set_icon_size(IconSize.SMALL_TOOLBAR);
        toolbar_box.set_homogeneous(false);
        toolbar_box.set_spacing(0);
        main_paned.set_position(500);

        files_paned.pack2(project_content_overlay, true, false);
        files_box.pack_start(files_control_box, false, false);
        files_box.pack_end(files_paned, true, true);
        toolbar_box.pack_start(toolbar, false, false);
        toolbar_box.pack_start(tool_separator, false, false);
        view_stack.add_named(toolbar_box, "map");
        main_paned.pack1(view_stack, true, false);
        main_paned.pack2(files_box, true, false);
        this.add(main_paned);
        this.set_titlebar(topbar);
    }

    private void init_content()
    {
        viewport = new GLArea();
        editor = new EditorEmbed(viewport);
        game = new GameEmbed(viewport);
        button_save = new Button.from_icon_name("document-save-symbolic", IconSize.SMALL_TOOLBAR);
        button_play = new ToggleButton();
        button_menu = new MenuButton();
        import_button = new Button.with_label("Import");
        filter_entry = new SearchEntry();
        project_tree_view = new TreeView();
        project_icon_view = new IconView();
        icon_scale = new Scale.with_range(Orientation.HORIZONTAL, 1, 6, 1);

        // Tree data stuff
        // Columns:
        // 0: Name
        // 1: URI
        // 2: Icon
        // 3: Is Folder
        // 4: Icon Name
        project_tree_data = new TreeStore(5, typeof(string), typeof(string), typeof(Gdk.Pixbuf), typeof(bool), typeof(string));
        project_tree_data.set_sort_column_id(0, SortType.ASCENDING);
        project_tree_filter = new TreeModelFilter(project_tree_data, null);
        project_tree_filter.set_visible_column(3);
        project_tree_view.set_model(project_tree_filter);
        project_tree_view.set_headers_visible(false);
        build_content_data();
        TreeViewColumn col_name = new Gtk.TreeViewColumn.with_attributes("Name", new CellRendererText(), "text", 0, null);
        project_tree_view.insert_column(col_name, -1);
        project_icon_view.text_column = 0;
        project_icon_view.pixbuf_column = 2;
        project_icon_view.item_width = 64;
        foreach(weak CellRenderer cell in project_icon_view.get_cells()) {
            project_icon_view.set_cell_data_func(cell, iconview_cell_data);
        }
        icon_scale.restrict_to_fill_level = true;
        icon_scale.draw_value = false;
        icon_scale.add_mark(4, PositionType.BOTTOM, null);
        icon_scale.halign = Align.END;
        icon_scale.valign = Align.END;
        icon_scale.hexpand = false;
        icon_scale.vexpand = false;
        icon_scale.width_request = 128;
        icon_scale.set_value(4);
        icon_scale.get_style_context().add_class(STYLE_CLASS_OSD);

        button_draw = new ToggleToolButton();
        button_fill = new ToggleToolButton();
        button_erase = new ToggleToolButton();

        button_play.set_image(new Gtk.Image.from_icon_name("media-playback-start-symbolic", IconSize.SMALL_TOOLBAR));
        button_menu.set_image(new Gtk.Image.from_icon_name("emblem-system-symbolic", IconSize.SMALL_TOOLBAR));
        button_menu.set_direction(ArrowType.DOWN);

        button_draw.set_icon_name("insert-object-symbolic");
        button_fill.set_icon_name("zoom-fit-best-symbolic");
        button_erase.set_icon_name("edit-clear-all-symbolic");

        import_button.image = new Image.from_icon_name("document-open-symbolic", IconSize.SMALL_TOOLBAR);
        import_button.get_style_context().add_class(STYLE_CLASS_SUGGESTED_ACTION);
        filter_entry.placeholder_text = "Search\u2026";
        filter_entry.editable = false;

        current_embed = editor;
        viewport.has_depth_buffer = true;
        viewport.auto_render = true;
        viewport.can_focus = true;
        viewport.set_required_version(3, 3);

        topbar.pack_start(button_save);
        topbar.pack_end(button_menu);
        topbar.pack_end(button_play);
        files_control_box.pack_start(filter_entry, false, false);
        files_control_box.pack_start(import_button, false, false);
        files_paned.pack1(project_tree_view, false, false);
        project_content_overlay.add(project_icon_view);
        project_content_overlay.add_overlay(icon_scale);
        toolbar_box.pack_start(viewport, true, true);
        toolbar.insert(button_draw,  0);
        toolbar.insert(button_erase, 1);
        toolbar.insert(button_fill,  2);
    }

    private void connect_signals()
    {
        this.set_events(Gdk.EventMask.ALL_EVENTS_MASK);
        this.destroy.connect(on_exit);

        // FIXME: New button doesn't load the new project correctly
        button_save.clicked.connect(() =>
        {
            if(current_map_name.length == 0) {
                current_map_path = get_selected_path();
                current_map_name = Halberd.IO.get_unique_name(current_map_path, "Untitled.map");
            }
            Game.Maps.save(current_map_path, current_map_name);
        });
        button_play.toggled.connect(() =>
        {
            if(button_play.active) {
                current_embed = game;
                game.prepare();
                viewport.grab_focus();
            } else {
                current_embed = editor;
            }

            this.show_all();
        });

        button_draw.clicked.connect(() => { editor.current_tool = Tool.DRAW; });
        button_erase.clicked.connect(() => { editor.current_tool = Tool.ERASE; });
        button_fill.clicked.connect(() => { editor.current_tool = Tool.FILL; });

        project_tree_view.cursor_changed.connect(() =>
        {
            TreePath path;
            TreeIter iter;
            string uri;
            project_tree_view.get_cursor(out path, null);
            if(path == null)
                return;
            project_tree_filter.get_iter(out iter, path);
            project_tree_filter.get(iter, 1, out uri);
            path = project_tree_filter.convert_path_to_child_path(path);
            project_content_data.get_iter(out iter, path);

            File dir_file = File.new_for_uri(uri);
            if(content_monitor != null)
                content_monitor.cancel();
            try {
                content_monitor = dir_file.monitor_directory(FileMonitorFlags.NONE);
                content_monitor.set_rate_limit(1000);
                content_monitor.changed.connect((dir, file, type) =>
                {
                    TreePath current_path = project_content_data.virtual_root;
                    TreeIter current_iter;
                    project_tree_data.get_iter(out current_iter, current_path);

                    switch(type) {
                        case FileMonitorEvent.CREATED:
                            try {
                                add_project_file(dir.query_info("standard::*", FileQueryInfoFlags.NONE), current_iter);
                            } catch(Error e) {
                                app.display_warning("Can't get file info: " + e.message);
                            }
                        break;

                        case FileMonitorEvent.DELETED:
                            rm_project_file(dir, current_iter);
                        break;
                    }
                });
            } catch(IOError e) {
                app.display_warning("Can't watch content directory: " + e.message);
            }

            build_content_data(path);
        });

        project_icon_view.item_activated.connect((path) =>
        {
            TreeIter iter;
            string file_name;
            string file_uri;
            bool is_folder = false;
            project_content_data.get_iter(out iter, path);
            project_content_data.get(iter, 0, out file_name, 1, out file_uri, 3, out is_folder);
            if(is_folder) {
                TreePath new_path = project_tree_filter.convert_child_path_to_path(project_content_data.convert_path_to_child_path(path));
                project_tree_view.expand_to_path(new_path);
                project_tree_view.set_cursor(new_path, null, false);
            } else {
                // TODO: Make this open differently for different filetypes
                current_map_path = get_selected_path();
                current_map_name = file_name;
                Halberd.Game.Maps.load(current_map_path, current_map_name);
                viewport.queue_draw();
            }
        });
        CellRendererText text = (CellRendererText)project_icon_view.get_cells().nth_data(0);
        text.edited.connect((path, new_text) =>
        {
            TreeIter iter;
            string file_name;
            string file_uri;
            project_content_data.get_iter(out iter, new TreePath.from_string(path));
            project_content_data.get(iter, 0, out file_name, 1, out file_uri);
            if(file_name == new_text) {
                text.editable = false;
                return;
            }

            File f = File.new_for_uri(file_uri);
            try {
                f = f.set_display_name(new_text);
            } catch(Error e) {
                app.display_warning("Error renaming directory: " + e.message);
            }
        });

        project_icon_view.button_press_event.connect((event) =>
        {
            if(event.button == 3) {
                TreePath path = project_icon_view.get_path_at_pos((int)event.x, (int)event.y);
                if(path != null) {
                    project_icon_view.select_path(path);
                    project_icon_view.set_cursor(path, null, false);
                } else {
                    project_icon_view.unselect_all();
                }
                project_icon_view.popup_menu();
            }
            return false;
        });
        project_icon_view.popup_menu.connect(() =>
        {
            if(project_icon_view.get_selected_items().length() == 0)
                content_menu.popup(null, null, null, 0, Gtk.get_current_event_time());
            else
                content_file_menu.popup(null, null, null, 0, Gtk.get_current_event_time());
            return false;
        });

        icon_scale.value_changed.connect(() =>
        {
            int new_value = (int)icon_scale.get_value();
            if(new_value == 4)
                new_value = 5;
            project_icon_view.item_width = new_value * 10 + 16;
            icon_size = new_value;
        });

        // TODO: File Previews
        import_button.clicked.connect(import_dialog);

        viewport.set_events(Gdk.EventMask.POINTER_MOTION_MASK | Gdk.EventMask.BUTTON_PRESS_MASK | Gdk.EventMask.BUTTON_RELEASE_MASK | Gdk.EventMask.KEY_PRESS_MASK | Gdk.EventMask.KEY_RELEASE_MASK | Gdk.EventMask.SCROLL_MASK | Gdk.EventMask.LEAVE_NOTIFY_MASK);
        viewport.realize.connect(editor.prepare);
        viewport.button_press_event.connect((event) => { return current_embed.button_down(event); });
        viewport.button_release_event.connect((event) => { return current_embed.button_up(event); });
        viewport.key_press_event.connect((event) => { return current_embed.key_down(event); });
        viewport.key_release_event.connect((event) => { return current_embed.key_up(event); });
        viewport.leave_notify_event.connect((event) => { return current_embed.wrap_cursor(event); });
        viewport.motion_notify_event.connect((event) => { return current_embed.move_cursor(event); });
        viewport.render.connect(() => { return current_embed.render(); });
        viewport.resize.connect((w, h) => { Editor.size_callback(w, h); });
        viewport.scroll_event.connect((event) => { return current_embed.scroll_cursor(event);});
    }

    /*
     * This function adds functions as GTK actions to various parts of the app,
     * for use in menus and with accelerators.
     * It also connects the activation signals on these actions.
     */
    private void add_actions()
    {
        // Main menu group
        SimpleActionGroup main_group = new SimpleActionGroup();
        SimpleAction act_new_project = new SimpleAction("new", null);
        act_new_project.activate.connect(app.new_project);
        main_group.add_action(act_new_project);
        SimpleAction act_load_project = new SimpleAction("load", null);
        act_load_project.activate.connect(app.load_dialog);
        main_group.add_action(act_load_project);
        SimpleAction act_exit = new SimpleAction("exit", null);
        act_exit.activate.connect(() => { this.destroy(); });
        main_group.add_action(act_exit);
        SimpleAction act_preferences = new SimpleAction("preferences", null);
        // TODO: Implement this
        act_preferences.set_enabled(false);
        main_group.add_action(act_preferences);
        SimpleAction act_about = new SimpleAction("about", null);
        act_about.activate.connect(() =>
        {
            app.display_about();
        });
        main_group.add_action(act_about);
        button_menu.insert_action_group("file", main_group);

        // Main content browser group
        SimpleActionGroup content_group = new SimpleActionGroup();
        SimpleAction act_import = new SimpleAction("import", null);
        act_import.activate.connect(import_dialog);
        act_import.set_enabled(true);
        content_group.add_action(act_import);
        SimpleAction act_open = new SimpleAction("open", null);
        act_open.activate.connect(() =>
        {
            project_icon_view.selected_foreach((view, path) =>
            {
                view.item_activated(path);
            });
        });
        content_group.add_action(act_open);
        SimpleAction act_rename = new SimpleAction("rename", null);
        act_rename.activate.connect(() =>
        {
            TreePath path = null;
            CellRenderer renderer;
            CellRendererText text = (CellRendererText)project_icon_view.get_cells().nth_data(0);
            project_icon_view.get_cursor(out path, out renderer);
            if(path != null) {
                text.editable = true;
                project_icon_view.set_cursor(path, text, true);
                text.editable = false;
            }
        });
        content_group.add_action(act_rename);
        SimpleAction act_delete = new SimpleAction("delete", null);
        act_delete.activate.connect(() =>
        {
            project_icon_view.selected_foreach((view, path) =>
            {
                TreeIter iter;
                string file_uri;
                project_content_data.get_iter(out iter, path);
                project_content_data.get(iter, 1, out file_uri);
                // Get the file
                File file = File.new_for_uri(file_uri);
                try {
                    file.delete();
                } catch(Error e) {
                    app.display_warning("Can't delete file: " + e.message);
                }
            });
        });
        content_group.add_action(act_delete);
        files_paned.insert_action_group("content", content_group);

        // Main content browser group for creating assets
        SimpleActionGroup content_new_group = new SimpleActionGroup();
        SimpleAction act_new_folder = new SimpleAction("folder", null);
        act_new_folder.activate.connect(() =>
        {
            try {
                File new_folder = file_from_resource(get_selected_path(), "Untitled", true);
                new_folder.make_directory();
            } catch(Error e) {
                app.display_warning("Can't make directory: " + e.message);
            }
        });
        content_new_group.add_action(act_new_folder);
        SimpleAction act_new_actor = new SimpleAction("actor", null);
        // TODO: Implement this
        act_new_actor.set_enabled(false);
        content_new_group.add_action(act_new_actor);
        SimpleAction act_new_map = new SimpleAction("map", null);
        // TODO: Implement this
        act_new_map.set_enabled(false);
        content_new_group.add_action(act_new_map);
        SimpleAction act_new_sprite = new SimpleAction("sprite", null);
        // TODO: Implement this
        act_new_sprite.set_enabled(false);
        content_new_group.add_action(act_new_sprite);
        SimpleAction act_new_tileset = new SimpleAction("tileset", null);
        // TODO: Implement this
        act_new_tileset.set_enabled(false);
        content_new_group.add_action(act_new_tileset);
        files_paned.insert_action_group("new", content_new_group);
    }

    /*
     * This function creates the menu models for this window, and also connects
     * the signals that display them.
     */
    private void init_menus()
    {
        // TODO: Implement this
        GLib.Menu main_model = new GLib.Menu();
        main_model.append("New Project", "file.new");
        main_model.append("Load Project", "file.load");

        GLib.Menu pref_section = new GLib.Menu();
        pref_section.append("Preferences", "file.preferences");
        main_model.append_section(null, pref_section);
        GLib.Menu app_section = new GLib.Menu();
        app_section.append("About", "file.about");
        app_section.append("Quit", "file.exit");
        main_model.append_section(null, app_section);
        button_menu.set_menu_model(main_model);


        GLib.Menu content_model = new GLib.Menu();
        content_model.append("Import File", "content.import");
        content_model.append("New Folder", "new.folder");

        GLib.Menu new_sub = new GLib.Menu();
        new_sub.append("Actor", "new.actor");
        new_sub.append("Map", "new.map");
        new_sub.append("Sprite", "new.sprite");
        new_sub.append("Tileset", "new.tileset");
        content_model.append_submenu("New", new_sub);

        GLib.Menu content_file_model = new GLib.Menu();
        content_file_model.append("Open", "content.open");
        content_file_model.append("Rename", "content.rename");
        content_file_model.append("Delete", "content.delete");

        content_menu = new Gtk.Menu.from_model(content_model);
        content_menu.attach_to_widget(files_paned, null);
        content_file_menu = new Gtk.Menu.from_model(content_file_model);
        content_file_menu.attach_to_widget(files_paned, null);
    }

    private void add_project_directory(File f, TreeIter? parent_iter)
    {
        TreeIter iter;
        project_tree_data.append(out iter, parent_iter);

        project_tree_data.set(iter, 0, f.get_basename(), 1, f.get_uri(), 2, folder_icon, 3, true, 4, "folder");
        FileInfo info;
        try{
            info = f.query_info ("standard::*", 0);
            FileEnumerator enumerator = f.enumerate_children (FileAttribute.STANDARD_NAME, 0);

            FileInfo file_info;
            while ((file_info = enumerator.next_file ()) != null) {
                add_project_file(file_info, iter);
            }
        } catch (Error e) {
            stderr.printf ("Error reading project directory: %s\n", e.message);
            return;
        }
    }

    /*
     * This function adds a new file to the treestore representing the content
     * directory
     */
    private void add_project_file(FileInfo info, TreeIter? parent_iter)
    {
        if(info.get_file_type () == FileType.DIRECTORY && parent_iter != null) {
            string uri;
            project_tree_data.get(parent_iter, 1, out uri);
            File f = File.new_for_uri(uri);

            add_project_directory(f.resolve_relative_path (info.get_name ()), parent_iter);
            return;
        }

        TreeIter temp_iter;
        project_tree_data.append(out temp_iter, parent_iter);

        string name = "text-x-generic";
        if(info.get_name().has_suffix(".png"))
            name = "image-x-generic";
        project_tree_data.set(temp_iter, 0, info.get_name(), 1, app.get_content_directory().get_uri() + info.get_name(), 2, default_icon, 3, false, 4, name);
    }

    /*
     * This function removes file file from the treestore representing the
     * content directory
     */
    private void rm_project_file(File to_remove, TreeIter? parent_iter)
    {
        if(parent_iter == null)
            project_tree_data.get_iter_first(out parent_iter);
        TreeIter child;
        project_tree_data.iter_children(out child, parent_iter);

        do {
            string uri;
            project_tree_data.get(child, 1, out uri);
            if(uri == to_remove.get_uri()) {
                // FIXME: What happens when the current directory is removed?
                project_tree_data.remove(ref child);
                return;
            }
        } while(project_tree_data.iter_next(ref child));
    }

    private void build_content_data(TreePath? path = null)
    {
        project_content_data = new TreeModelFilter(project_tree_data, path);
        project_icon_view.set_model(project_content_data);
    }

    private void iconview_cell_data(CellLayout layout, CellRenderer cell, TreeModel model, TreeIter iter)
    {
        if(cell is CellRendererPixbuf) {
            string icon_name = "folder";
            model.get(iter, 4, out icon_name);
            ((CellRendererPixbuf)cell).icon_name = icon_name;
            ((CellRendererPixbuf)cell).stock_size = icon_size;
            //cell.width = 100;
            //cell.height = 100;
        } else if(cell is CellRendererText) {
            string title = "";
            model.get(iter, 0, out title);
            ((CellRendererText)cell).text = title;
        }
    }

    private void import_dialog()
    {
        Gtk.FileChooserDialog fc = new Gtk.FileChooserDialog("Import an Asset", this, Gtk.FileChooserAction.OPEN, "Cancel", Gtk.ResponseType.CANCEL, "Open", Gtk.ResponseType.ACCEPT);
        Gtk.FileFilter filter = new Gtk.FileFilter();
        filter.add_pattern("*.png");
        filter.set_filter_name("PNG files");
        fc.add_filter(filter);
        fc.response.connect((r) =>
        {
            if(r == Gtk.ResponseType.ACCEPT) {
                try {
                    File dest = File.new_for_path(app.get_content_directory().get_path() + "/" + get_selected_path() + Halberd.IO.get_unique_name(get_selected_path(), fc.get_file().get_basename()));
                    fc.get_file().copy(dest, FileCopyFlags.NONE);
                } catch(Error e) {
                    stderr.printf("Error importing: %s\n", e.message);
                }
            }
            fc.destroy();
        });
        fc.run();
    }

    private void on_exit()
    {
        Editor.cleanup_render();
        app.window_destroyed(this);
    }
}
