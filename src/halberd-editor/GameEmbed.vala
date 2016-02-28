using Halberd;
using Gtk;

enum InputAction
{
    ACTION = 0,
    CONFIRM,
    CANCEL,
    MENU,
    QUIT,
    ACTION_COUNT
}

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
            DF.Input.set_axis(0, false, -1, false, false);
        if(event.keyval == down_key)
            DF.Input.set_axis(0, false, 1, false, false);
        if(event.keyval == left_key)
            DF.Input.set_axis(0, true, -1, false, false);
        if(event.keyval == right_key)
            DF.Input.set_axis(0, true, 1, false, false);
        if(event.keyval == action_key)
            DF.Input.set_state(InputAction.ACTION, true);
        if(event.keyval == confirm_key)
            DF.Input.set_state(InputAction.CONFIRM, true);
        if(event.keyval == cancel_key)
            DF.Input.set_state(InputAction.CANCEL, true);
        if(event.keyval == menu_key)
            DF.Input.set_state(InputAction.MENU, true);
        if(event.keyval == quit_key)
            DF.Input.set_state(InputAction.QUIT, true);
        return false;
    }

    public override bool key_up(Gdk.EventKey event)
    {
        if(event.keyval == up_key)
            DF.Input.set_axis(0, false, 1, false, false);
        if(event.keyval == down_key)
            DF.Input.set_axis(0, false, -1, false, false);
        if(event.keyval == left_key)
            DF.Input.set_axis(0, true, 1, false, false);
        if(event.keyval == right_key)
            DF.Input.set_axis(0, true, -1, false, false);
        if(event.keyval == action_key)
            DF.Input.set_state(InputAction.ACTION, false);
        if(event.keyval == confirm_key)
            DF.Input.set_state(InputAction.CONFIRM, false);
        if(event.keyval == cancel_key)
            DF.Input.set_state(InputAction.CANCEL, false);
        if(event.keyval == menu_key)
            DF.Input.set_state(InputAction.MENU, false);
        if(event.keyval == quit_key)
            DF.Input.set_state(InputAction.QUIT, false);
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
        DF.Input.update(fdelta);

        viewport.queue_draw();
        return should_continue;
    }
}
