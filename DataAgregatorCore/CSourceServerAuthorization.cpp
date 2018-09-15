/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CSourceServerAuthorization.h"


const QString& CSourceServerAuthorization::g_hostField("host");
const QString& CSourceServerAuthorization::g_loginField("login");
const QString& CSourceServerAuthorization::g_passwordField("password");
const QString& CSourceServerAuthorization::g_keyField("keyFile");

CSourceServerAuthorization::CSourceServerAuthorization()
{

}

CSourceServerAuthorization::CSourceServerAuthorization(const QVariantMap& sourceServerAuthorizationMap)
  : m_sourceServerAuthorizationMap(sourceServerAuthorizationMap)
{
}

CSourceServerAuthorization::CSourceServerAuthorization(const QString& host, const QString& login, const QString& password, const QString& key)
  : m_sourceServerAuthorizationMap({
                                            {g_hostField, host},
                                            {g_loginField, login},
                                            {g_passwordField, password},
                                            {g_keyField, key}
                                          })
{

}

QString CSourceServerAuthorization::host() const
{
  return m_sourceServerAuthorizationMap[g_hostField].toString();
}

QString CSourceServerAuthorization::login() const
{
  return m_sourceServerAuthorizationMap[g_loginField].toString();
}

QString CSourceServerAuthorization::password() const
{
    return m_sourceServerAuthorizationMap[g_passwordField].toString();
}

QString CSourceServerAuthorization::keyFile() const
{
    return m_sourceServerAuthorizationMap[g_keyField].toString();
}

bool CSourceServerAuthorization::authorizationByKey() const
{
    return password().isEmpty();
}

bool CSourceServerAuthorization::authorizationByPassword() const
{
    return keyFile().isEmpty();
}

const QVariantMap& CSourceServerAuthorization::sourceServerAuthorizationMap() const
{
  return m_sourceServerAuthorizationMap;
}

