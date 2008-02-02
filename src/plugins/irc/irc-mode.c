/*
 * Copyright (c) 2003-2008 by FlashCode <flashcode@flashtux.org>
 * See README for License detail, AUTHORS for developers list.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* irc-mode.c: IRC channel/user modes management */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "../weechat-plugin.h"
#include "irc.h"
#include "irc-mode.h"
#include "irc-server.h"
#include "irc-channel.h"
#include "irc-nick.h"


/*
 * irc_mode_channel_set_nick: set a mode for a nick on a channel
 */

void
irc_mode_channel_set_nick (struct t_irc_channel *channel, char *nick,
                           char set_flag, int flag)
{
    (void) channel;
    (void) nick;
    (void) set_flag;
    (void) flag;
    
    /*
    struct t_irc_nick *ptr_nick;
    struct t_gui_nick *ptr_gui_nick;
    int sort_index, color_prefix;
    char prefix;
    
    if (nick)
    {
        ptr_nick = irc_nick_search (channel, nick);
        if (ptr_nick)
        {
            IRC_NICK_SET_FLAG(ptr_nick, (set_flag == '+'), flag);
            ptr_gui_nick = gui_nicklist_search (channel->buffer,
                                                ptr_nick->nick);
            if (ptr_gui_nick)
            {
                irc_nick_get_gui_infos (ptr_nick, &sort_index, &prefix,
                                        &color_prefix);
                gui_nicklist_update (channel->buffer, ptr_gui_nick, NULL,
                                     sort_index,
                                     ptr_gui_nick->color_nick,
                                     prefix,
                                     color_prefix);
                gui_nicklist_draw (channel->buffer, 1, 1);
            }
        }
    }
    */
}

/*
 * irc_mode_channel_get_flag: search for flag before current position
 */

char
irc_mode_channel_get_flag (char *str, char *pos)
{
    char set_flag;

    set_flag = '+';
    pos--;
    while (pos >= str)
    {
        if (pos[0] == '-')
            return '-';
        if (pos[0] == '+')
            return '+';
        pos--;
    }
    return set_flag;
}

/*
 * irc_mode_channel_set: set channel modes
 */

void
irc_mode_channel_set (struct t_irc_server *server,
                      struct t_irc_channel *channel, char *modes)
{
    char *pos_args, set_flag, **argv, *pos, *ptr_arg;
    int argc, current_arg;
    
    argc = 0;
    argv = NULL;
    current_arg = 0;
    pos_args = strchr (modes, ' ');
    if (pos_args)
    {
        pos_args[0] = '\0';
        pos_args++;
        while (pos_args[0] == ' ')
            pos_args++;
        argv = weechat_string_explode (pos_args, " ", 0, 0, &argc);
        if (argc > 0)
            current_arg = argc - 1;
    }
    
    if (modes && modes[0])
    {
        set_flag = '+';
        pos = modes + strlen (modes) - 1;
        while (pos >= modes)
        {
            switch (pos[0])
            {
                case ':':
                case ' ':
                case '+':
                case '-':
                    break;
                default:
                    set_flag = irc_mode_channel_get_flag (modes, pos);
                    switch (pos[0])
                    {
                        case 'a': /* channel admin (unrealircd specific flag) */
                            ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                argv[current_arg--] : NULL;
                            if (irc_mode_nick_prefix_allowed (server, '~'))
                                irc_mode_channel_set_nick (channel, ptr_arg,
                                                           set_flag, IRC_NICK_CHANADMIN);
                            break;
                        case 'b': /* ban (ignored) */
                            ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                argv[current_arg--] : NULL;
                            break;
                        case 'h': /* half-op */
                            ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                argv[current_arg--] : NULL;
                            if (irc_mode_nick_prefix_allowed (server, '%'))
                                irc_mode_channel_set_nick (channel, ptr_arg,
                                                           set_flag, IRC_NICK_HALFOP);
                            break;
                        case 'k': /* channel key */
                            if (channel->key)
                            {
                                free (channel->key);
                                channel->key = NULL;
                            }
                            if (set_flag == '+')
                            {
                                ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                    argv[current_arg--] : NULL;
                                if (ptr_arg)
                                    channel->key = strdup (ptr_arg);
                            }
                            break;
                        case 'l': /* channel limit */
                            if (set_flag == '-')
                                channel->limit = 0;
                            if (set_flag == '+')
                            {
                                ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                    argv[current_arg--] : NULL;
                                if (ptr_arg)
                                    channel->limit = atoi (ptr_arg);
                            }
                            break;
                        case 'o': /* op */
                            ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                argv[current_arg--] : NULL;
                            if (irc_mode_nick_prefix_allowed (server, '@'))
                                irc_mode_channel_set_nick (channel, ptr_arg,
                                                           set_flag, IRC_NICK_OP);
                            break;
                        case 'q': /* channel owner (unrealircd specific flag) */
                            ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                argv[current_arg--] : NULL;
                            if (irc_mode_nick_prefix_allowed (server, '~'))
                                irc_mode_channel_set_nick (channel, ptr_arg,
                                                           set_flag, IRC_NICK_CHANOWNER);
                            break;
                        case 'u': /* channel user */
                            ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                argv[current_arg--] : NULL;
                            if (irc_mode_nick_prefix_allowed (server, '-'))
                                irc_mode_channel_set_nick (channel, ptr_arg,
                                                           set_flag, IRC_NICK_CHANUSER);
                            break;
                        case 'v': /* voice */
                            ptr_arg = ((argc > 0) && (current_arg >= 0)) ?
                                argv[current_arg--] : NULL;
                            if (irc_mode_nick_prefix_allowed (server, '+'))
                                irc_mode_channel_set_nick (channel, ptr_arg,
                                                           set_flag, IRC_NICK_VOICE);
                            break;
                    }
                    break;
            }
            pos--;
        }
    }
    
    if (argv)
        weechat_string_free_exploded (argv);
}

/*
 * irc_mode_user_add: add a user mode
 */

void
irc_mode_user_add (struct t_irc_server *server, char mode)
{
    char str_mode[2];

    str_mode[0] = mode;
    str_mode[1] = '\0';
    
    if (server->nick_modes)
    {
        if (!strchr (server->nick_modes, mode))
        {
            server->nick_modes = (char *)realloc (server->nick_modes,
                                                  (strlen (server->nick_modes) + 1 + 1) * sizeof (char));
            strcat (server->nick_modes, str_mode);
            //gui_status_draw (gui_current_window->buffer, 1);
            //gui_input_draw (gui_current_window->buffer, 1);
        }
    }
    else
    {
        server->nick_modes = (char *)malloc (2 * sizeof (char));
        strcpy (server->nick_modes, str_mode);
        //gui_status_draw (gui_current_window->buffer, 1);
        //gui_input_draw (gui_current_window->buffer, 1);
    }
}

/*
 * irc_mode_user_remove: remove a user mode
 */

void
irc_mode_user_remove (struct t_irc_server *server, char mode)
{
    char *pos;
    int new_size;
    
    if (server->nick_modes)
    {
        pos = strchr (server->nick_modes, mode);
        if (pos)
        {
            new_size = strlen (server->nick_modes);
            memmove (pos, pos + 1, strlen (pos + 1) + 1);
            server->nick_modes = (char *)realloc (server->nick_modes,
                                                  new_size * sizeof (char));
            //gui_status_draw (gui_current_window->buffer, 1);
            //gui_input_draw (gui_current_window->buffer, 1);
        }
    }
}

/*
 * irc_mode_user_set: set user modes
 */

void
irc_mode_user_set (struct t_irc_server *server, char *modes)
{
    char set_flag;
    
    set_flag = '+';
    while (modes && modes[0])
    {
        switch (modes[0])
        {
            case ':':
            case ' ':
                break;
            case '+':
                set_flag = '+';
                break;
            case '-':
                set_flag = '-';
                break;
            default:
                if (set_flag == '+')
                    irc_mode_user_add (server, modes[0]);
                else
                    irc_mode_user_remove (server, modes[0]);
                break;
        }
        modes++;
    }
}

/*
 * irc_mode_nick_prefix_allowed: return <> 0 if nick prefix is allowed by server
 *                               for example :
 *                                 IRC:  005 (...) PREFIX=(ov)@+
 *                               => allowed prefixes: @+
 */

int
irc_mode_nick_prefix_allowed (struct t_irc_server *server, char prefix)
{
    char str[2];
    
    /* if server did not send any prefix info, then use default prefixes */
    if (!server->prefix)
    {
        str[0] = prefix;
        str[1] = '\0';
        return (strpbrk (str, IRC_NICK_DEFAULT_PREFIXES_LIST)) ? 1 : 0;
    }
    
    return (strchr (server->prefix, prefix) != NULL);
}
