class HalberdEditor : Gtk.Application
{
    MainWindow window;

    public HalberdEditor(string[] args)
    {
        window = new MainWindow();
    }

    public int runall()
    {
        Gtk.main();
        return 0;
    }
}
