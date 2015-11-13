using Gtk;
using Halberd;

enum Tool
{
    NONE,
    DRAW,
    LINE,
    FILL,
    RECT,
    OVAL,
    ERASE,
}

public class MainWindow: Window
{
    public AccelGroup accel;

    private HeaderBar toolbar;

    private GLArea viewport;

    private Gtk.Button button_new;
    private Gtk.Button button_open;
    private Gtk.Button button_draw;
    private Gtk.Button button_fill;
    private Gtk.Button button_erase;

    private bool dragging_map = false;
    private bool dragging_line = false;
    private Tool current_tool = Tool.DRAW;

    public MainWindow()
    {
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
        viewport.realize.connect(prepare_viewport);
        viewport.render.connect(render_viewport);
        viewport.has_depth_buffer = true;
        viewport.auto_render = true;
        viewport.set_required_version(3, 3);
        viewport.set_events(Gdk.EventMask.POINTER_MOTION_MASK | Gdk.EventMask.BUTTON_PRESS_MASK | Gdk.EventMask.BUTTON_RELEASE_MASK | Gdk.EventMask.SCROLL_MASK);
        viewport.resize.connect((w, h) => {
            Editor.size_callback(w, h);
        });
        viewport.motion_notify_event.connect((event) => {
            if(dragging_map)
                Editor.Cursor.drag_map((int)event.x, (int)event.y);
            else if(dragging_line) {
                if(current_tool == Tool.DRAW)
                    Editor.Cursor.place_line(1, (int)event.x, (int)event.y);
                else if(current_tool == Tool.ERASE)
                    Editor.Cursor.place_line(0, (int)event.x, (int)event.y);
            }
            Editor.Cursor.set_position((int)event.x, (int)event.y);
            viewport.queue_draw();
            return false;
        });
        viewport.button_press_event.connect((event) => {
            if(event.button == 2)
                dragging_map = true;
            else if(event.button == 1) {
                switch(current_tool) {
                    case Tool.DRAW:
                        Editor.Cursor.place_tile(1);
                        dragging_line = true;
                        break;
                    case Tool.FILL:
                        Editor.Cursor.flood_fill(1);
                        break;
                    case Tool.ERASE:
                        Editor.Cursor.place_tile(0);
                        dragging_line = true;
                        break;
                }
            }
            viewport.queue_draw();
            return false;
        });
        viewport.button_release_event.connect((event) => {
            if(event.button == 2)
                dragging_map = false;
            else if(event.button == 1) {
                dragging_line = false;
            }
            return false;
        });
        viewport.scroll_event.connect((event) => {
            // TODO: Implement this
            double x, y;
            if(!event.get_scroll_deltas(out x, out y)) {
                x = event.x;
                y = event.y;
                switch(event.direction) {
                    case Gdk.ScrollDirection.UP:
                        y = -1;
                    break;
                    case Gdk.ScrollDirection.DOWN:
                        y = 1;
                    break;
                    case Gdk.ScrollDirection.LEFT:
                        x = -1;
                    break;
                    case Gdk.ScrollDirection.RIGHT:
                        x = 1;
                    break;
                }
            }
            if((event.state & Gdk.ModifierType.CONTROL_MASK) != 0) {
                Editor.Cursor.zoom(-y);
                viewport.queue_draw();
            }
            return false;
        });

        button_new = new Gtk.Button.from_icon_name("document-new-symbolic", IconSize.LARGE_TOOLBAR);
        button_open = new Gtk.Button.from_icon_name("document-open-symbolic", IconSize.LARGE_TOOLBAR);
        button_draw = new Gtk.Button.from_icon_name("insert-object-symbolic", IconSize.LARGE_TOOLBAR);
        button_draw.clicked.connect(() => { current_tool = Tool.DRAW; });
        button_fill = new Gtk.Button.from_icon_name("zoom-fit-best-symbolic", IconSize.LARGE_TOOLBAR);
        button_fill.clicked.connect(() => { current_tool = Tool.FILL; });
        button_erase = new Gtk.Button.from_icon_name("edit-clear-all-symbolic", IconSize.LARGE_TOOLBAR);
        button_erase.clicked.connect(() => { current_tool = Tool.ERASE; });

        toolbar.pack_start(button_new);
        toolbar.pack_start(button_open);
        toolbar.pack_start(button_draw);
        toolbar.pack_start(button_fill);
        toolbar.pack_start(button_erase);

        this.set_titlebar(toolbar);
        this.add(viewport);

        this.window_position = WindowPosition.CENTER;
        this.set_default_size(1024, 768);
        this.set_border_width(3);
        this.destroy.connect(on_exit);
        this.show_all();
    }

    private void on_exit()
    {
        Editor.cleanup_render();
        Gtk.main_quit();
    }

    private void prepare_viewport()
    {
        viewport.make_current();
        Editor.init_render();
        // TODO: refactor this to send a signal to the main app, rather than doing stuff here
        Editor.Automaps.init();
        Game.Maps.load("test");
        Editor.Automaps.load("test");
    }

    private bool render_viewport()
    {
        return Editor.render();
    }
}
