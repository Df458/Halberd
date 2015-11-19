using Gtk;
using Halberd;

public class MainWindow : Window
{
    public AccelGroup accel;

    private HeaderBar toolbar;

    private EditorEmbed editor;
    private GameEmbed game;
    private EmbeddableView current_embed;

    private Gtk.GLArea viewport;

    private Gtk.Button button_new;
    private Gtk.Button button_open;
    private Gtk.Button button_save;
    private Gtk.Button button_draw;
    private Gtk.Button button_fill;
    private Gtk.Button button_erase;

    private Gtk.ToggleButton button_play;

    public MainWindow()
    {
        window_position = WindowPosition.CENTER;
        set_default_size(1024, 768);
        set_border_width(3);

        destroy.connect(on_exit);
        accel = new Gtk.AccelGroup();
        add_accel_group(accel);
        set_events(Gdk.EventMask.ALL_EVENTS_MASK);

        // Init. Structure
        toolbar = new Gtk.HeaderBar();
        toolbar.title = "Halberd RPG Engine";
        toolbar.show_close_button = true;
        toolbar.decoration_layout = "menu:close";

        // Init. Widgets
        viewport = new GLArea();
        editor = new EditorEmbed(viewport);
        game = new GameEmbed(viewport);
        current_embed = editor;

        viewport.has_depth_buffer = true;
        viewport.auto_render = true;
        viewport.can_focus = true;
        viewport.set_required_version(3, 3);
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

        button_new = new Gtk.Button.from_icon_name("document-new-symbolic", IconSize.LARGE_TOOLBAR);
        button_new.sensitive = false;

        button_open = new Gtk.Button.from_icon_name("document-open-symbolic", IconSize.LARGE_TOOLBAR);
        button_open.sensitive = false;

        button_save = new Gtk.Button.from_icon_name("document-save-symbolic", IconSize.LARGE_TOOLBAR);
        button_save.clicked.connect(() => { Editor.Automaps.save(); Game.Maps.save(); });

        button_draw = new Gtk.Button.from_icon_name("insert-object-symbolic", IconSize.LARGE_TOOLBAR);
        button_draw.clicked.connect(() => { editor.current_tool = Tool.DRAW; });

        button_fill = new Gtk.Button.from_icon_name("zoom-fit-best-symbolic", IconSize.LARGE_TOOLBAR);
        button_fill.clicked.connect(() => { editor.current_tool = Tool.FILL; });

        button_erase = new Gtk.Button.from_icon_name("edit-clear-all-symbolic", IconSize.LARGE_TOOLBAR);
        button_erase.clicked.connect(() => { editor.current_tool = Tool.ERASE; });

        button_play = new Gtk.ToggleButton();
        button_play.set_image(new Gtk.Image.from_icon_name("media-playback-start-symbolic", IconSize.LARGE_TOOLBAR));
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

        toolbar.pack_start(button_new);
        toolbar.pack_start(button_open);
        toolbar.pack_start(button_save);
        toolbar.pack_start(button_draw);
        toolbar.pack_start(button_fill);
        toolbar.pack_start(button_erase);
        toolbar.pack_start(button_play);

        this.set_titlebar(toolbar);
        this.add(viewport);
        this.show_all();
    }

    private void on_exit()
    {
        Editor.cleanup_render();
        Gtk.main_quit();
    }
}
