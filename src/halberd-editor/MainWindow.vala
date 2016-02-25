using Gtk;
using Halberd;

public class MainWindow : Window
{
    public AccelGroup accel;

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

    // Project               Pane
    private ProjectFilePane project_view;
    private DF.LogPane         log_view;

    // Inspector Pane
    private SidePane inspector_pane;

    //                       Editors
    private BlankEditor      empty_view;
    private ActorEditor      actor_view;
    private SpriteEditor     sprite_view;

    // Containers
    private Paned        main_paned;
    private Paned        inspector_paned;
    private Box          toolbar_box;
    private Gtk.Stack    view_stack;
    private Gtk.Notebook bottom_notebook;

    string current_map_name = "";
    string? current_map_path = null;

    public MainWindow()
    {
        init_structure();
        init_content();
        connect_signals();
        add_actions();
        init_menus();

        view_stack.set_visible_child(empty_view);

        this.show_all();
    }

    public void reload()
    {
        topbar.subtitle = Editor.get_loaded_project_name();
        // TODO: Reload the editor here
    }

    // TODO: This should probably be moved out of this class
    public void load_file(string? resource_path, string resource_name)
    {
        unowned string? ext = DF.IO.get_extension(resource_name);
        switch(ext) {
            case "map":
                view_stack.set_visible_child(inspector_paned);
                Halberd.Game.Maps.load(current_map_path, current_map_name);
                current_map_path = resource_path;
                current_map_name = resource_name;
                for(uint8 i = 0; i < Halberd.Game.Maps.get_tileset_count(); ++i) {
                    string? path = Halberd.Game.Maps.get_tileset_location(i);
                    string name = Halberd.Game.Maps.get_tileset_name(i);
                    inspector_pane.add_tileset(new ResourceEntry.from_entry(path, name));
                }
                viewport.queue_draw();
                break;
            case "actr":
                view_stack.set_visible_child(actor_view);
                actor_view.load(new ResourceEntry.from_entry(resource_path, resource_name));
                break;
            case "spr":
                view_stack.set_visible_child(sprite_view);
                sprite_view.load(new ResourceEntry.from_entry(resource_path, resource_name));
                break;
            default:
                view_stack.set_visible_child(empty_view);
                break;
        }

    }

    private void init_structure()
    {
        topbar          = new HeaderBar();
        main_paned      = new Paned(Orientation.VERTICAL);
        inspector_paned = new Paned(Orientation.HORIZONTAL);
        toolbar_box     = new Box(Orientation.HORIZONTAL, 0);
        tool_separator  = new Separator(Orientation.VERTICAL);
        toolbar         = new Toolbar();
        view_stack      = new Gtk.Stack();
        bottom_notebook = new Notebook();

        this.window_position = WindowPosition.CENTER;
        this.set_default_size(1024, 768);
        accel = new Gtk.AccelGroup();
        this.add_accel_group(accel);
        topbar.title = "Halberd RPG Engine";
        topbar.subtitle = Editor.get_loaded_project_name();
        topbar.show_close_button = true;
        topbar.decoration_layout = "menu:close";
        toolbar.orientation_changed(Orientation.VERTICAL);
        toolbar.set_icon_size(IconSize.SMALL_TOOLBAR);
        toolbar_box.set_homogeneous(false);
        toolbar_box.set_spacing(0);
        main_paned.set_position(500);

        toolbar_box.pack_start(toolbar, false, false);
        toolbar_box.pack_start(tool_separator, false, false);
        inspector_paned.pack1(toolbar_box, true, false);
        view_stack.add_named(inspector_paned, "map");
        main_paned.pack1(view_stack, true, false);
        main_paned.pack2(bottom_notebook, true, false);
        this.add(main_paned);
        this.set_titlebar(topbar);
    }

    private void init_content()
    {
        viewport       = new GLArea();
        editor         = new EditorEmbed(viewport);
        game           = new GameEmbed(viewport);
        button_save    = new Button.from_icon_name("document-save-symbolic", IconSize.SMALL_TOOLBAR);
        button_play    = new ToggleButton();
        button_menu    = new MenuButton();
        project_view   = new ProjectFilePane();
        inspector_pane = new SidePane();
        empty_view     = new BlankEditor();
        actor_view     = new ActorEditor();
        sprite_view    = new SpriteEditor();
        log_view       = new DF.LogPane();

        button_draw  = new ToggleToolButton();
        button_fill  = new ToggleToolButton();
        button_erase = new ToggleToolButton();

        button_play.set_image(new Gtk.Image.from_icon_name("media-playback-start-symbolic", IconSize.SMALL_TOOLBAR));
        button_menu.set_image(new Gtk.Image.from_icon_name("emblem-system-symbolic", IconSize.SMALL_TOOLBAR));
        button_menu.set_direction(ArrowType.DOWN);

        button_draw.set_icon_name("insert-object-symbolic");
        button_fill.set_icon_name("zoom-fit-best-symbolic");
        button_erase.set_icon_name("edit-clear-all-symbolic");

        log_view.set_active();

        current_embed = editor;
        viewport.has_depth_buffer = true;
        viewport.auto_render = true;
        viewport.can_focus = true;
        viewport.set_required_version(3, 3);

        view_stack.add_named(empty_view, "");
        view_stack.add_named(actor_view, "act");
        view_stack.add_named(sprite_view, "spr");
        topbar.pack_start(button_save);
        topbar.pack_end(button_menu);
        topbar.pack_end(button_play);
        inspector_paned.pack2(inspector_pane, false, true);
        bottom_notebook.append_page(project_view, new Label("Content"));
        bottom_notebook.append_page(log_view, new Label("Debug Log"));
        project_view.show_all();
        bottom_notebook.set_current_page(0);
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
            switch(view_stack.visible_child_name) {
                case "map":
                    if(current_map_name.length == 0) {
                        current_map_path = project_view.get_selected_path();
                        current_map_name = DF.IO.get_unique_name(current_map_path, "Untitled.map");
                    }
                    Game.Maps.save(current_map_path, current_map_name);
                    break;
                case "act":
                    actor_view.save();
                    break;
                case "spr":
                    sprite_view.save();
                    break;
            }
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

        project_view.file_opened.connect((path, name) => { load_file(path, name); });
        project_view.file_import.connect(import_dialog);
        project_view.file_new.connect((type) =>
        {
            string name = "Untitled";
            AssetEditor selected_editor = null;
            switch(type) {
                case AssetType.ACTOR:
                    name = name.concat(".actr");
                    selected_editor = actor_view;
                break;
            }

            if(selected_editor == null)
                return;

            ResourceEntry entry = new ResourceEntry.from_entry(project_view.get_selected_path(), name, true);
            selected_editor.create(entry);
        });

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
    }

    /*
     * This function creates the menu models for this window, and also connects
     * the signals that display them.
     */
    private void init_menus()
    {
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
    }

    // TODO: Move this out of the class
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
                    File dest = File.new_for_path(DF.IO.get_content_directory().get_path() + "/" + project_view.get_selected_path() + "/" + DF.IO.get_unique_name(project_view.get_selected_path(), fc.get_file().get_basename()));
                    fc.get_file().copy(dest, FileCopyFlags.NONE);
                } catch(Error e) {
                    DF.Logger.log_error("Error importing asset: %s\n", e.message);
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
