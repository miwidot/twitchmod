#ifndef PREDICTIONDIALOG_H
#define PREDICTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVector>

class PredictionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PredictionDialog(QWidget *parent = nullptr);

    struct PredictionData {
        QString title;
        QStringList outcomes;
        int durationSeconds;
    };

    PredictionData getPredictionData() const;

private slots:
    void onAddOutcome();
    void onRemoveOutcome();
    void onCreateClicked();

private:
    void setupUI();
    void updateRemoveButton();

    QLineEdit *m_titleEdit;
    QSpinBox *m_durationSpinBox;
    QVBoxLayout *m_outcomesLayout;
    QVector<QLineEdit*> m_outcomeEdits;
    QPushButton *m_addOutcomeButton;
    QPushButton *m_removeOutcomeButton;
    QPushButton *m_createButton;
    QPushButton *m_cancelButton;

    static constexpr int MIN_OUTCOMES = 2;
    static constexpr int MAX_OUTCOMES = 10;
};

#endif // PREDICTIONDIALOG_H
