/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/
#include "kcm_kmail.cpp"
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(KMailSecurityConfigFactory, "kmail_config_security.json", registerPlugin<SecurityPage>();)

#include "kcm_kmail_security.moc"
