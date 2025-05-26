/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   built_in.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pandemonium <pandemonium@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 14:33:18 by lflayeux          #+#    #+#             */
/*   Updated: 2025/05/26 11:28:14 by pandemonium      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"

// ==================
// ======= PWD ======
// ==================

void    exec_pwd()
{
    char *path;
    path = getcwd(NULL, 0);
    printf(YLW"%s\n"RST, path);
    free(path);
}
// ==================
// ======= CD =======
// ==================

void    exec_cd(char *path)
{
    char    *join;

    if (path[0] == '~')
    {
        join = ft_strjoin(getenv("HOME"), path + 1);
        printf("%s\n", join);
        chdir(join);
    }
    else
        chdir(path);
    exec_pwd();
}
// ==================
// ======= ECHO =====
// ==================

void exec_echo(t_exec **exec, int *i)
{
    int newline;
    int j;

    (*i)++;
    newline = 1;
    while ((*exec)->cmd[*i])
    {
        if (ft_strncmp((*exec)->cmd[*i], "-n", 2) == 0)
        {
            j = 2;
            while ((*exec)->cmd[*i][j] == 'n')
                j++;
            if ((*exec)->cmd[*i][j] == '\0')
            {
                newline = 0;
                (*i)++;
                continue;
            }
        }
        break;
    }
    while ((*exec)->cmd[*i])
    {
        printf("%s", (*exec)->cmd[*i]);
        if ((*exec)->cmd[*i + 1])
            printf(" ");
        (*i)++;
    }
    if (newline)
        printf("\n");
}
int is_valid_env(char *exec)
{
    int i = 0;

    if (!exec || (!ft_isalpha(exec[0]) && exec[0] != '_'))
        return (0);
    while (exec[i] && exec[i] != '=')
    {
        if (!ft_isalnum(exec[i]) && exec[i] != '_')
            return (0);
        i++;
    }
    return (1);
}
// ========================
// ======= RESET ENV ======
// ========================
void set_env(char ***env, char **split, char *to_check)
{
    int i = 0;

    if (!split || !split[0])
        return ;

    while ((*env)[i])
    {
        if (ft_strncmp((*env)[i], split[0], ft_strlen(split[0])) == 0 && (*env)[i][ft_strlen(split[0])] == '=')
            break;
        i++;
    }
    if ((*env)[i] != NULL)
    {
        free((*env)[i]);
        (*env)[i] = ft_strdup(to_check);
    }
}

// ========================
// ======= UNSET ENV ======
// ========================
void    unset_env(char ***env, char *unset_env)
{
    char **new_env;
    int len;
    int i;

    i = 0;
    len = 0;
    while ((*env)[len])
        len++;
    new_env = ft_calloc(len + 1, sizeof(char *));
    if (new_env == NULL)
        return ;
    while (env[i])
    {
        if (ft_strncmp((*env)[i], unset_env, ft_strlen(unset_env)) == 0)
            i++;
        new_env[i] = ft_strdup((*env)[i]);
        i++;
    }
    ft_free_tab((void **)(*env));
    *env = new_env;
}

void    exec_unset(t_exec	**exec, int *i, char ***env)
{
    char **split;

    while ((*exec)->cmd[*i + 1])
    {
        if (!is_valid_env((*exec)->cmd[*i + 1]))
        {
            printf("INVALID ENV: %s\n", (*exec)->cmd[*i + 1]);
            return ;
        }
        split = ft_split((*exec)->cmd[*i + 1], '=');
        if (!getenv(split[0]))
            return ;
        else
            unset_env(env, (*exec)->cmd[*i + 1]);
        (*i)++;
    }
}

// ========================
// ======= SET NEW ENV ======
// ========================


void put_env(char ***env, const char *new_var)
{
    int len;
    int i;
    char **new_env;

    len = 0;
    i = 0;
    while ((*env)[len])
        len++;
    new_env = ft_calloc(len + 2, sizeof(char *));
    if (!new_env)
        return;
    while (i < len)
    {
        new_env[i] = ft_strdup((*env)[i]);
        i++;
    }
    new_env[i] = ft_strdup(new_var);
    new_env[i + 1] = NULL;
    ft_free_tab((void **)(*env));
    *env = new_env;
}



void    check_env(char ***env, char *to_check)
{
    char **split;

    split = ft_split(to_check, '=');
    if (!split || !split[0])
    {
        ft_free_tab((void **)split);
        return ;
    }
    if (getenv(split[0]))
    {
        set_env(env, split, to_check);
        ft_free_tab((void **)split);
        return ;
    }
    put_env(env, to_check);
    ft_free_tab((void **)split);
}

void exec_env(t_exec **exec, int *i, char ***env)
{
    int j;

    j = 0;
    while ((*exec)->cmd[*i + 1])
    {
        if (ft_strchr((*exec)->cmd[*i + 1], '='))
        {
            if (!is_valid_env((*exec)->cmd[*i + 1]))
                printf("INVALID ENV: %s'\n", (*exec)->cmd[*i + 1]);
            else
                check_env(env, (*exec)->cmd[*i + 1]);
        }
        else
            break;
        (*i)++;
    }
    while ((*env)[j])
    {
        printf("%s\n", (*env)[j]);
        j++;
    }
    (*i)++;
}


void    exec_export(t_exec	**exec, int *i, char ***env)
{
    int j;
    char **split;

    j = 0;
    if (!(*exec)->cmd[*i + 1])
    {
        while ((*env)[j])
            printf("declare -x %s\n", (*env)[j++]);
    }
    while ((*exec)->cmd[*i + 1])
    {
        if (!is_valid_env((*exec)->cmd[*i + 1]))
            printf("INVALID ENV: %s\n", (*exec)->cmd[*i + 1]);
        else
        {
            split = ft_split((*exec)->cmd[*i + 1], '=');
            if (getenv(split[0]))
                set_env(env, split, (*exec)->cmd[*i + 1]);
            else
                put_env(env, (*exec)->cmd[*i + 1]);
            ft_free_tab((void**)split);
        }
        (*i)++;
    }
}

// POUR EXPORT SI PAS DE = ON EXPORT A PART

void    built_in(t_exec	**exec, int *i, char ***env)
{
    if (!(ft_strcmp((*exec)->cmd[*i], "echo")))
        exec_echo(exec, i);
    else if (ft_strcmp((*exec)->cmd[*i], "cd") == 0)
         exec_cd((*exec)->cmd[++(*i)]);
    else if (ft_strcmp((*exec)->cmd[*i], "pwd") == 0)
        exec_pwd();
    else if (ft_strcmp((*exec)->cmd[*i], "env") == 0)
        exec_env(exec, i, env);
    else if (ft_strcmp((*exec)->cmd[*i], "export") == 0)
        exec_export(exec, i, env);
   else if (ft_strcmp((*exec)->cmd[*i], "unset") == 0)
       exec_unset(exec, i, env);
    else
        return ;
}
