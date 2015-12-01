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
    private Button button_new;
    private Button button_open;
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

    // Containers
    private Paned main_paned;
    private Paned files_paned;
    private Box   files_box;
    private Box   files_control_box;
    private Box   toolbar_box;
    private Gtk.Stack view_stack;

    string current_map_name = "";

    public MainWindow()
    {
        init_structure();
        init_content();
        connect_signals();
        read_project_directory();
        this.show_all();
    }

    public void reload()
    {
        topbar.subtitle = Editor.get_loaded_project_name();
        // TODO: Reload the editor here
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

        files_box.pack_start(files_control_box, false, false);
        files_box.pack_end(files_paned, true, true);
        toolbar_box.pack_start(toolbar, false, false);
        toolbar_box.pack_start(tool_separator, false, false);
        view_stack.add_named(toolbar_box, "map");
        main_paned.pack1(view_stack, true, false);
        main_paned.pack2(files_box, false, false);
        this.add(main_paned);
        this.set_titlebar(topbar);
    }

    private void init_content()
    {
        viewport = new GLArea();
        editor = new EditorEmbed(viewport);
        game = new GameEmbed(viewport);
        button_new  = new Button.from_icon_name("document-new-symbolic",  IconSize.SMALL_TOOLBAR);
        button_open = new Button.from_icon_name("document-open-symbolic", IconSize.SMALL_TOOLBAR);
        button_save = new Button.from_icon_name("document-save-symbolic", IconSize.SMALL_TOOLBAR);
        button_play = new ToggleButton();
        button_menu = new MenuButton();
        import_button = new Button.with_label("Import");
        filter_entry = new SearchEntry();
        project_tree_view = new TreeView();
        project_icon_view = new IconView();

        // Tree data stuff
        project_tree_data = new TreeStore(2, typeof(string), typeof(string));
        project_tree_data.set_sort_column_id(0, SortType.ASCENDING);
        project_tree_view.set_model(project_tree_data);
        TreeViewColumn col_name = new Gtk.TreeViewColumn.with_attributes("Name", new CellRendererText(), "text", 0, null);
        project_tree_view.insert_column(col_name, -1);

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

        topbar.pack_start(button_new);
        topbar.pack_start(button_open);
        topbar.pack_start(button_save);
        topbar.pack_end(button_menu);
        topbar.pack_end(button_play);
        files_control_box.pack_start(filter_entry, false, false);
        files_control_box.pack_start(import_button, false, false);
        files_paned.pack1(project_tree_view, false, false);
        files_paned.pack2(project_icon_view, true, false);
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
        button_new.clicked.connect(app.new_project);
        button_open.clicked.connect(app.load_dialog);
        button_save.clicked.connect(() => {
            if(current_map_name.length == 0) {
                // TODO: Make the names selectable ahead of time
                current_map_name = "test";
            }
            Game.Maps.save(current_map_name);
        });
        button_play.toggled.connect(() => {
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

    private void read_project_directory()
    {
        // TODO: Implement this
    }

    private void on_exit()
    {
        Editor.cleanup_render();
        app.window_destroyed(this);
    }
}
