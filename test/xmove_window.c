/*
 * Simple test program to create an X Window and move it with a keyboard.
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static int xPos = 0;
static int yPos = 0;

static void
eventLoop(Display *dpy, Window win);

static Window create_window(
    Display*            dpy,
    XVisualInfo*        vis_info,
    int                 x,
    int                 y,
    int                 width,
    int                 height);

int
main(int argc, char **argv)
{
    Display *dpy;
    XVisualInfo template;
    XVisualInfo* vis_info;
    Window window;
    int nitems;

    /* Open display */
    if (!(dpy = XOpenDisplay(NULL))) {
        fprintf(stderr, "Could not open display\n");
        exit(1);
    }

    template.class = TrueColor;
    template.depth = 24;
    vis_info = XGetVisualInfo(dpy, VisualClassMask | VisualDepthMask,
                              &template, &nitems);

    if (nitems < 1 || vis_info == NULL) {
        fprintf(stderr, "No conforming visual exists\n");
        exit(1);
    }

    fprintf(stderr,
        "found a %d-bit visual (visual ID = 0x%x)\n",
        vis_info->depth,
        (unsigned int)vis_info->visualid);

    window = create_window(dpy, vis_info,
                           10, 10, 100, 100);
    eventLoop(dpy, window);
}


static void
eventLoop(Display *dpy, Window win)
{
    XEvent event, next_event;

    while (1) {
        XNextEvent(dpy, &event);
        switch(event.type) {
        case VisibilityNotify:
            {
                XVisibilityEvent *xve = (XVisibilityEvent*) &event;
                fprintf(stderr,
                        "Visibility event: visibility = %d\n",
                        xve->state);
                break;
            }
        case Expose:
            {
                XExposeEvent *xee = (XExposeEvent*) &event;
                fprintf(stderr, "Expose event: (%d, %d) %dx%d\n",
                        xee->x, xee->y, xee->width, xee->height);
                break;
            }
        case ConfigureNotify:
            {
                XConfigureEvent *xce = (XConfigureEvent*) &event;
                fprintf(stderr, "ConfigureNotify event\n");
                break;
            }

        case KeyPress:
            {
                int i, j;
                char text[3];

                fprintf(stderr, "KeyPress event: ");
                i = XLookupString((XKeyEvent *)&event,
                                  text,
                                  sizeof(text),
                                  NULL,
                                  NULL);
                if (i == 1) {
                    fprintf(stderr, "key = 0x%x ('%c')", text[0], text[0]);
                    switch (text[0]) {
                        case '-':
                            xPos -= 10;
                            yPos -= 10;
                            fprintf(stderr, "move to: %d, %d\n", xPos, yPos);
                            XMoveWindow(dpy, win, xPos, yPos);
                            break;

                        case '+':
                        case '=':
                            xPos += 10;
                            yPos += 10;
                            fprintf(stderr, "move to: %d, %d\n", xPos, yPos);
                            XMoveWindow(dpy, win, xPos, yPos);
                            break;

                        case '\033':
                            fprintf(stderr, "\n");
                            exit(0);
                    }
                }
                fprintf(stderr, "\n");
                break;
            }
        case MappingNotify:
            XRefreshKeyboardMapping((XMappingEvent *)&event);
            break;
        }
    }
}


static Window
create_window(
    Display*            dpy,
    XVisualInfo*        vis_info,
    int                 x,
    int                 y,
    int                 width,
    int                 height)
{
    Window                      window;
    XSetWindowAttributes        wattrs;
    XEvent                      event;
    Colormap                    cmap;

    xPos = x;
    yPos = y;

    cmap = XCreateColormap(dpy, RootWindow(dpy, vis_info->screen),
                vis_info->visual, AllocNone);

    /* Create the output window.  */
    wattrs.background_pixel = 0;
    wattrs.border_pixel = 0;
    wattrs.override_redirect = True;
    wattrs.colormap = cmap;
    wattrs.event_mask = VisibilityChangeMask | ExposureMask | StructureNotifyMask | KeyPressMask;
    window = XCreateWindow(dpy, RootWindow(dpy, vis_info->screen),
        x, y, width, height, 1,
        vis_info->depth, InputOutput, vis_info->visual,
        CWBackPixel | CWBorderPixel | CWEventMask | CWColormap, &wattrs);
    XMapWindow(dpy, window );

    return window;
}
