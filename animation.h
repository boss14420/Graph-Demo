/*
 * =====================================================================================
 *
 *       Filename:  animation.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/06/2012 01:11:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  BOSS14420 (boss14420), boss14420@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _ANIMATON_H_
#define _ANIMATON_H_

#include <QPropertyAnimation>

QPropertyAnimation *create(QObject *object, 
                    const QByteArray& propertyName,
                    QVariant const &startValue,
                    QVariant const &endValue,
                    int duration = 200);

#endif
