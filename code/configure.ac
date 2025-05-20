#Configurations script to find programs needed to translate 

AC_INIT([maze_game], [2025], [oliver.dangl@st.oth-regensburg.de]) #name program, year, email for contact
AC_CONFIG_SRCDIR([main.c])
AM_INIT_AUTOMAKE([foreign]) #project is without gnu standards


AC_PROG_CC #C program


# Finding GTK+ 3.0 with pkg-config
PKG_CHECK_MODULES([GTK], [gtk+-3.0])
AC_SUBST([GTK_CFLAGS])
AC_SUBST([GTK_LIBS])


AC_CONFIG_FILES([Makefile]) #Which files need to be configured
AC_OUTPUT #Generating output of the file
