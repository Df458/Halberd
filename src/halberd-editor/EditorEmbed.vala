using Halberd;
using Gtk;

public enum Tool
{
    NONE,
    DRAW,
    LINE,
    FILL,
    RECT,
    OVAL,
    ERASE,
}

public class EditorEmbed : EmbeddableView
{
    private bool dragging_map = false;
    private bool drag_wrap = false;
    private bool dragging_line = false;
    private bool first_run = true;
    public Tool current_tool = Tool.DRAW;

    private GLArea viewport;

    public EditorEmbed(GLArea area)
    {
        viewport = area;
    }

    public void prepare()
    {
        viewport.make_current();
        if(first_run) {
            Editor.init_render();
            Editor.Automaps.init();
        }
        editor_ready();
        viewport.queue_draw();
        first_run = false;
    }

    public signal void editor_ready();

    public override bool button_down(Gdk.EventButton event)
    {
        if(event.button == 2)
            dragging_map = true;
        else if(event.button == 1) {
            switch(current_tool) {
                case Tool.DRAW:
                    Editor.Cursor.place_tile();
                    dragging_line = true;
                    break;
                case Tool.FILL:
                    Editor.Cursor.flood_fill(1);
                    break;
                case Tool.ERASE:
                    Editor.Cursor.place_tile_id(0);
                    dragging_line = true;
                    break;
            }
        }
        viewport.queue_draw();
        return false;
    }

    public override bool button_up(Gdk.EventButton event)
    {
        if(event.button == 2)
            dragging_map = false;
        else if(event.button == 1) {
            dragging_line = false;
        }
        return false;
    }

    public override bool key_down(Gdk.EventKey event)
    {
        return false;
    }

    public override bool key_up(Gdk.EventKey event)
    {
        return false;
    }

    public override bool move_cursor(Gdk.EventMotion event)
    {
        if(dragging_map) {
            if(drag_wrap)
                drag_wrap = false;
            else
                Editor.Cursor.drag_map((int)event.x, (int)event.y);
        } else if(dragging_line) {
            if(current_tool == Tool.DRAW)
                Editor.Cursor.place_line((int)event.x, (int)event.y);
            else if(current_tool == Tool.ERASE)
                Editor.Cursor.place_line_id(0, (int)event.x, (int)event.y);
        }
        Editor.Cursor.set_position((int)event.x, (int)event.y);
        viewport.queue_draw();
        return false;
    }

    public override bool scroll_cursor(Gdk.EventScroll event)
    {
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
    }

    public override bool wrap_cursor(Gdk.EventCrossing event)
    {
        if(!dragging_map)
            return false;
        int w, h, wx, wy;
        int dx = (int)event.x_root;
        int dy = (int)event.y_root;
        event.window.get_origin(out wx, out wy);
        event.window.get_geometry(null, null, out w, out h);
        if(event.x > (double)w) {
            dx -= w;
        } else if(event.x < 0) {
            dx += w;
        }
        if(event.y > (double)h) {
            dy -= h;
        } else if(event.y < 0) {
            dy += h;
        }
        event.get_device().warp(event.window.get_screen(), dx, dy);
        drag_wrap = true;
        return false;
    }

    public override bool render()
    {
        return Editor.render();
    }
}
