#ifndef GLOBALVAR_H
#define GLOBALVAR_H

#ifdef PLUGIN
//disable using global variables in plugins
#define GLOBALVAR(x)
#else
#define GLOBALVAR(x) extern x
#endif

#endif // GLOBALVAR_H
