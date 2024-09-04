#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <QString>
#include <QVector>

/**
 * @brief Structure representing a user.
 */
struct User
{
    int id;           /**< User ID. */
    QString userType; /**< Type of user (e.g., "teacher", "pupil"). */
};

/**
 * @brief Global variable representing the current user logged in.
 * 
 * This variable is used to store information about the current user
 * who is logged into the application.
 */
extern User currentUser;

/**
 * @brief Structure representing a pupil.
 */
struct Pupil
{
    int id;               /**< Pupil ID. */
    QString name;         /**< Pupil's name. */
    QVector<int> results; /**< Results of the pupil for various levels or tests. */
};

/**
 * @brief Structure representing a question.
 */
struct Question
{
    QString name;     /**< Name associated with the question. */
    QString question; /**< The question itself. */
    QString date;     /**< Date when the question was asked or created. */
};

#endif // GLOBAL_VARS_H
