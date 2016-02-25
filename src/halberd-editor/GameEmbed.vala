using Halberd;
using Gtk;

public class GameEmbed : EmbeddableView
{
    private bool should_continue = true;
    private int64 time = 0;
    private bool first_run = true;

    private int up_key      = Gdk.Key.Up;
    private int down_key    = Gdk.Key.Down;
    private int left_key    = Gdk.Key.Left;
    private int right_key   = Gdk.Key.Right;
    private int action_key  = Gdk.Key.Z;
    private int confirm_key = Gdk.Key.Z;
    private int cancel_key  = Gdk.Key.X;
    private int menu_key    = Gdk.Key.C;
    private int quit_key    = Gdk.Key.Escape;

    private GLArea viewport;

    public GameEmbed(GLArea area)
    {
        viewport = area;
    }

    public void prepare()
    {
        Game.Player.init(0, 0);
        Game.UI.init();
        Game.Settings.init();
        time = get_monotonic_time();
        should_continue = true;
        Timeout.add(16, loop_step);
        viewport.queue_draw();
        first_run = false;
    }

    public override bool button_down(Gdk.EventButton event)
    {
        return false;
    }

    public override bool button_up(Gdk.EventButton event)
    {
        return false;
    }

    public override bool key_down(Gdk.EventKey event)
    {
        if(event.keyval == up_key)
            DF.Input.set_up_state(true);
        if(event.keyval == down_key)
            DF.Input.set_down_state(true);
        if(event.keyval == left_key)
            DF.Input.set_left_state(true);
        if(event.keyval == right_key)
            DF.Input.set_right_state(true);
        if(event.keyval == action_key)
            DF.Input.set_action_state(true);
        if(event.keyval == confirm_key)
            DF.Input.set_confirm_state(true);
        if(event.keyval == cancel_key)
            DF.Input.set_cancel_state(true);
        if(event.keyval == menu_key)
            DF.Input.set_menu_state(true);
        if(event.keyval == quit_key)
            DF.Input.set_quit_state(true);
        return false;
    }

    public override bool key_up(Gdk.EventKey event)
    {
        if(event.keyval == up_key)
            DF.Input.set_up_state(false);
        if(event.keyval == down_key)
            DF.Input.set_down_state(false);
        if(event.keyval == left_key)
            DF.Input.set_left_state(false);
        if(event.keyval == right_key)
            DF.Input.set_right_state(false);
        if(event.keyval == menu_key)
            DF.Input.set_menu_state(false);
        if(event.keyval == quit_key)
            DF.Input.set_quit_state(false);
        return false;
    }

    public override bool move_cursor(Gdk.EventMotion event)
    {
        return false;
    }

    public override bool scroll_cursor(Gdk.EventScroll event)
    {
        return false;
    }

    public override bool wrap_cursor(Gdk.EventCrossing event)
    {
        return false;
    }

    public override bool render()
    {
        Game.render();
        return false;
    }

    private bool loop_step()
    {
        int64 dt = time;
        time = get_monotonic_time();
        dt = time - dt;
        float fdelta = dt / 1000000.0f;

        Game.Player.update(fdelta);
        Game.Actors.update(fdelta);
        Game.UI.update(fdelta);
        DF.Input.update();

        viewport.queue_draw();
        return should_continue;
    }
}
