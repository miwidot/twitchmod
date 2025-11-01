#ifndef POLLDIALOG_H
#define POLLDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVector>

class PollDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PollDialog(QWidget *parent = nullptr);

    struct PollData {
        QString title;
        QStringList choices;
        int durationSeconds;
        bool channelPointsVotingEnabled;
    };

    PollData getPollData() const;

private slots:
    void onAddChoice();
    void onRemoveChoice();
    void onCreateClicked();

private:
    void setupUI();
    void updateRemoveButton();

    QLineEdit *m_titleEdit;
    QSpinBox *m_durationSpinBox;
    QCheckBox *m_channelPointsCheckbox;
    QVBoxLayout *m_choicesLayout;
    QVector<QLineEdit*> m_choiceEdits;
    QPushButton *m_addChoiceButton;
    QPushButton *m_removeChoiceButton;
    QPushButton *m_createButton;
    QPushButton *m_cancelButton;

    static constexpr int MIN_CHOICES = 2;
    static constexpr int MAX_CHOICES = 5;
};

#endif // POLLDIALOG_H
