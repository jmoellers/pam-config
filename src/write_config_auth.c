/* Copyright (C) 2006 Thorsten Kukuk
   Author: Thorsten Kukuk <kukuk@thkukuk.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "pam-config.h"

int
write_config_auth (const char *file, config_file_t *conf)
{
  FILE *fp;

  if (debug)
    printf ("*** write_config_auth (%s, ...)\n", file);

  /* XXX rename () */

  fp = fopen(file, "w");
  if (fp == NULL)
    {
      fprintf (stderr, _("Cannot create %s: %m\n"),
	       file);
      return -1;
    }

  fprintf (fp, "#%%PAM-1.0\n#\n");
  fprintf (fp, "# This file is autogenerated by pam-config. All changes\n");
  fprintf (fp, "# will be overwritten.\n#\n");
  fprintf (fp, "# Authentication-related modules common to all services\n#\n");
  fprintf (fp,
	   "# This file is included from other service-specific PAM config files,\n"
	   "# and should contain a list of the authentication modules that define\n"
	   "# the central authentication scheme for use on the system\n"
	   "# (e.g., /etc/shadow, LDAP, Kerberos, etc.). The default is to use the\n"
	   "# traditional Unix authentication mechanisms.\n#\n");

  if (conf->use_env)
    fprintf (fp, "auth\trequired\tpam_env.so\n");

  if (conf->use_pkcs11)
    fprintf (fp, "auth\tsufficient\tpam_pkcs11.so\n");

  if (conf->use_bioapi)
    {
      fprintf (fp, "auth\tsufficient\tpam_bioapi.so\t");
      if (conf->bioapi_options)
	fprintf (fp, "%s ", conf->bioapi_options);
      fprintf (fp, "\n");
    }

  if (conf->use_krb5 || conf->use_ldap || conf->use_lum)
    /* Only sufficient if other modules follow */
    fprintf (fp, "auth\tsufficient\tpam_unix2.so\t");
  else
    fprintf (fp, "auth\trequired\tpam_unix2.so\t");
  if (conf->unix2_nullok)
    fprintf (fp, "nullok ");
  if (conf->unix2_debug)
    fprintf (fp, "debug ");
  if (conf->unix2_call_modules)
    fprintf (fp, "call_modules=%s ", conf->unix2_call_modules);
  fprintf (fp, "\n");

  if (conf->use_krb5)
    {
      if (conf->use_ccreds)
	fprintf (fp, "auth\t[authinfo_unavail=ignore success=1 default=2]\tpam_krb5.so\tuse_first_pass ");
      else if (conf->use_ldap || conf->use_lum)
	fprintf (fp, "auth\tsufficient\tpam_krb5.so\tuse_first_pass ");
      else
	fprintf (fp, "auth\trequired\tpam_krb5.so\tuse_first_pass ");

      if (conf->krb5_debug)
	fprintf (fp, "debug ");
      fprintf (fp, "\n");

      if (conf->use_ccreds)
	{
	  fprintf (fp, "auth\t[default=done]\tpam_ccreds.so\taction=validate use_first_pass\n");
	  fprintf (fp, "auth\t[default=done]\tpam_ccreds.so\taction=store\n");
	  fprintf (fp, "auth\t[default=bad]\tpam_ccreds.so\taction=update\n");
	}
    }

  if (conf->use_ldap)
    {
      if (conf->use_ccreds)
	fprintf (fp, "auth\t[authinfo_unavail=ignore success=1 default=2]\tpam_ldap.so\tuse_first_pass ");
      else
	fprintf (fp, "auth\trequired\tpam_ldap.so\tuse_first_pass ");
      if (conf->ldap_debug)
	fprintf (fp, "debug ");
      fprintf (fp, "\n");
      if (conf->use_ccreds)
	{
	  fprintf (fp, "auth\t[default=done]\tpam_ccreds.so\taction=validate use_first_pass\n");
	  fprintf (fp, "auth\t[default=done]\tpam_ccreds.so\taction=store\n");
	  fprintf (fp, "auth\t[default=bad]\tpam_ccreds.so\taction=update\n");
	}
    }

  if (conf->use_lum)
    fprintf (fp, "auth\trequired\tpam_nam.so\tuse_first_pass\n");

  fclose (fp);

  return 0;
}
