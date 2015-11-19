public abstract class EmbeddableView
{
    public abstract bool button_down(Gdk.EventButton event);
    public abstract bool button_up(Gdk.EventButton event);
    public abstract bool key_down(Gdk.EventKey event);
    public abstract bool key_up(Gdk.EventKey event);
    public abstract bool move_cursor(Gdk.EventMotion event);
    public abstract bool scroll_cursor(Gdk.EventScroll event);
    public abstract bool wrap_cursor(Gdk.EventCrossing event);
    public abstract bool render();
}
