#include "predictiondialog.h"
#include <QFormLayout>
#include <QMessageBox>

PredictionDialog::PredictionDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Create Prediction");
    setMinimumWidth(500);
    setupUI();
}

void PredictionDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title input
    QGroupBox *titleGroup = new QGroupBox("Prediction Title", this);
    QVBoxLayout *titleLayout = new QVBoxLayout(titleGroup);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("e.g., Will we win this game?");
    m_titleEdit->setMaxLength(45); // Twitch limit
    titleLayout->addWidget(m_titleEdit);

    QLabel *titleHint = new QLabel("Maximum 45 characters", this);
    titleHint->setStyleSheet("color: gray; font-size: 11px;");
    titleLayout->addWidget(titleHint);

    mainLayout->addWidget(titleGroup);

    // Duration input
    QGroupBox *durationGroup = new QGroupBox("Duration", this);
    QHBoxLayout *durationLayout = new QHBoxLayout(durationGroup);

    m_durationSpinBox = new QSpinBox(this);
    m_durationSpinBox->setMinimum(15); // Twitch minimum
    m_durationSpinBox->setMaximum(1800); // Twitch maximum (30 minutes)
    m_durationSpinBox->setValue(60); // Default 1 minute
    m_durationSpinBox->setSuffix(" seconds");
    durationLayout->addWidget(m_durationSpinBox);

    QLabel *durationHint = new QLabel("(15s - 1800s)", this);
    durationHint->setStyleSheet("color: gray;");
    durationLayout->addWidget(durationHint);
    durationLayout->addStretch();

    mainLayout->addWidget(durationGroup);

    // Outcomes
    QGroupBox *outcomesGroup = new QGroupBox("Outcomes", this);
    QVBoxLayout *outcomesMainLayout = new QVBoxLayout(outcomesGroup);

    m_outcomesLayout = new QVBoxLayout();

    // Add initial 2 outcomes
    for (int i = 0; i < MIN_OUTCOMES; ++i) {
        QLineEdit *outcomeEdit = new QLineEdit(this);
        outcomeEdit->setPlaceholderText(QString("Outcome %1 (e.g., Yes / No)").arg(i + 1));
        outcomeEdit->setMaxLength(25); // Twitch limit
        m_outcomeEdits.append(outcomeEdit);
        m_outcomesLayout->addWidget(outcomeEdit);
    }

    outcomesMainLayout->addLayout(m_outcomesLayout);

    // Add/Remove outcome buttons
    QHBoxLayout *outcomeButtonsLayout = new QHBoxLayout();
    m_addOutcomeButton = new QPushButton("+ Add Outcome", this);
    m_removeOutcomeButton = new QPushButton("- Remove Outcome", this);

    outcomeButtonsLayout->addWidget(m_addOutcomeButton);
    outcomeButtonsLayout->addWidget(m_removeOutcomeButton);
    outcomeButtonsLayout->addStretch();

    outcomesMainLayout->addLayout(outcomeButtonsLayout);

    QLabel *outcomesHint = new QLabel("2-10 outcomes, max 25 characters each", this);
    outcomesHint->setStyleSheet("color: gray; font-size: 11px;");
    outcomesMainLayout->addWidget(outcomesHint);

    mainLayout->addWidget(outcomesGroup);

    // Dialog buttons
    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_createButton = new QPushButton("Create Prediction", this);
    m_createButton->setDefault(true);
    m_createButton->setStyleSheet("QPushButton { background-color: #9147ff; color: white; padding: 8px 16px; font-weight: bold; }");

    m_cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_createButton);

    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(m_addOutcomeButton, &QPushButton::clicked, this, &PredictionDialog::onAddOutcome);
    connect(m_removeOutcomeButton, &QPushButton::clicked, this, &PredictionDialog::onRemoveOutcome);
    connect(m_createButton, &QPushButton::clicked, this, &PredictionDialog::onCreateClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    updateRemoveButton();
}

void PredictionDialog::onAddOutcome()
{
    if (m_outcomeEdits.size() >= MAX_OUTCOMES) {
        return;
    }

    QLineEdit *outcomeEdit = new QLineEdit(this);
    outcomeEdit->setPlaceholderText(QString("Outcome %1").arg(m_outcomeEdits.size() + 1));
    outcomeEdit->setMaxLength(25);
    m_outcomeEdits.append(outcomeEdit);
    m_outcomesLayout->addWidget(outcomeEdit);

    updateRemoveButton();
}

void PredictionDialog::onRemoveOutcome()
{
    if (m_outcomeEdits.size() <= MIN_OUTCOMES) {
        return;
    }

    QLineEdit *lastOutcome = m_outcomeEdits.takeLast();
    m_outcomesLayout->removeWidget(lastOutcome);
    lastOutcome->deleteLater();

    updateRemoveButton();
}

void PredictionDialog::updateRemoveButton()
{
    m_removeOutcomeButton->setEnabled(m_outcomeEdits.size() > MIN_OUTCOMES);
    m_addOutcomeButton->setEnabled(m_outcomeEdits.size() < MAX_OUTCOMES);
}

void PredictionDialog::onCreateClicked()
{
    // Validate title
    if (m_titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a prediction title.");
        m_titleEdit->setFocus();
        return;
    }

    // Validate outcomes
    QStringList outcomes;
    for (QLineEdit *edit : m_outcomeEdits) {
        QString outcome = edit->text().trimmed();
        if (outcome.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "All outcomes must have a title.");
            edit->setFocus();
            return;
        }
        if (outcomes.contains(outcome, Qt::CaseInsensitive)) {
            QMessageBox::warning(this, "Invalid Input",
                               QString("Duplicate outcome: '%1'").arg(outcome));
            edit->setFocus();
            return;
        }
        outcomes.append(outcome);
    }

    accept();
}

PredictionDialog::PredictionData PredictionDialog::getPredictionData() const
{
    PredictionData data;
    data.title = m_titleEdit->text().trimmed();
    data.durationSeconds = m_durationSpinBox->value();

    for (QLineEdit *edit : m_outcomeEdits) {
        data.outcomes.append(edit->text().trimmed());
    }

    return data;
}
