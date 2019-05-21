#pragma once

// pure virtual class used for any cryo control user interfaces (ncurses, webserver..)
class control_ui
{
    public:

    virtual ~control_ui() = default;

    // add common functions as needed..

    protected:

    control_ui() = default;

};