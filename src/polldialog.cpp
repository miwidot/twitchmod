#include "polldialog.h"
#include <QFormLayout>
#include <QMessageBox>

PollDialog::PollDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Create Poll");
    setMinimumWidth(500);
    setupUI();
}

void PollDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title input
    QGroupBox *titleGroup = new QGroupBox("Poll Question", this);
    QVBoxLayout *titleLayout = new QVBoxLayout(titleGroup);

    m_titleEdit = new QLineEdit(this);
    m_titleEdit->setPlaceholderText("e.g., What should we play next?");
    m_titleEdit->setMaxLength(60); // Twitch limit
    titleLayout->addWidget(m_titleEdit);

    QLabel *titleHint = new QLabel("Maximum 60 characters", this);
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

    // Channel Points Voting
    m_channelPointsCheckbox = new QCheckBox("Enable Channel Points Voting", this);
    m_channelPointsCheckbox->setChecked(false);
    mainLayout->addWidget(m_channelPointsCheckbox);

    // Choices
    QGroupBox *choicesGroup = new QGroupBox("Choices", this);
    QVBoxLayout *choicesMainLayout = new QVBoxLayout(choicesGroup);

    m_choicesLayout = new QVBoxLayout();

    // Add initial 2 choices
    for (int i = 0; i < MIN_CHOICES; ++i) {
        QLineEdit *choiceEdit = new QLineEdit(this);
        choiceEdit->setPlaceholderText(QString("Choice %1").arg(i + 1));
        choiceEdit->setMaxLength(25); // Twitch limit
        m_choiceEdits.append(choiceEdit);
        m_choicesLayout->addWidget(choiceEdit);
    }

    choicesMainLayout->addLayout(m_choicesLayout);

    // Add/Remove choice buttons
    QHBoxLayout *choiceButtonsLayout = new QHBoxLayout();
    m_addChoiceButton = new QPushButton("+ Add Choice", this);
    m_removeChoiceButton = new QPushButton("- Remove Choice", this);

    choiceButtonsLayout->addWidget(m_addChoiceButton);
    choiceButtonsLayout->addWidget(m_removeChoiceButton);
    choiceButtonsLayout->addStretch();

    choicesMainLayout->addLayout(choiceButtonsLayout);

    QLabel *choicesHint = new QLabel("2-5 choices, max 25 characters each", this);
    choicesHint->setStyleSheet("color: gray; font-size: 11px;");
    choicesMainLayout->addWidget(choicesHint);

    mainLayout->addWidget(choicesGroup);

    // Dialog buttons
    mainLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_createButton = new QPushButton("Create Poll", this);
    m_createButton->setDefault(true);
    m_createButton->setStyleSheet("QPushButton { background-color: #9147ff; color: white; padding: 8px 16px; font-weight: bold; }");

    m_cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_createButton);

    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(m_addChoiceButton, &QPushButton::clicked, this, &PollDialog::onAddChoice);
    connect(m_removeChoiceButton, &QPushButton::clicked, this, &PollDialog::onRemoveChoice);
    connect(m_createButton, &QPushButton::clicked, this, &PollDialog::onCreateClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    updateRemoveButton();
}

void PollDialog::onAddChoice()
{
    if (m_choiceEdits.size() >= MAX_CHOICES) {
        return;
    }

    QLineEdit *choiceEdit = new QLineEdit(this);
    choiceEdit->setPlaceholderText(QString("Choice %1").arg(m_choiceEdits.size() + 1));
    choiceEdit->setMaxLength(25);
    m_choiceEdits.append(choiceEdit);
    m_choicesLayout->addWidget(choiceEdit);

    updateRemoveButton();
}

void PollDialog::onRemoveChoice()
{
    if (m_choiceEdits.size() <= MIN_CHOICES) {
        return;
    }

    QLineEdit *lastChoice = m_choiceEdits.takeLast();
    m_choicesLayout->removeWidget(lastChoice);
    lastChoice->deleteLater();

    updateRemoveButton();
}

void PollDialog::updateRemoveButton()
{
    m_removeChoiceButton->setEnabled(m_choiceEdits.size() > MIN_CHOICES);
    m_addChoiceButton->setEnabled(m_choiceEdits.size() < MAX_CHOICES);
}

void PollDialog::onCreateClicked()
{
    // Validate title
    if (m_titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a poll question.");
        m_titleEdit->setFocus();
        return;
    }

    // Validate choices
    QStringList choices;
    for (QLineEdit *edit : m_choiceEdits) {
        QString choice = edit->text().trimmed();
        if (choice.isEmpty()) {
            QMessageBox::warning(this, "Invalid Input", "All choices must have a title.");
            edit->setFocus();
            return;
        }
        if (choices.contains(choice, Qt::CaseInsensitive)) {
            QMessageBox::warning(this, "Invalid Input",
                               QString("Duplicate choice: '%1'").arg(choice));
            edit->setFocus();
            return;
        }
        choices.append(choice);
    }

    accept();
}

PollDialog::PollData PollDialog::getPollData() const
{
    PollData data;
    data.title = m_titleEdit->text().trimmed();
    data.durationSeconds = m_durationSpinBox->value();
    data.channelPointsVotingEnabled = m_channelPointsCheckbox->isChecked();

    for (QLineEdit *edit : m_choiceEdits) {
        data.choices.append(edit->text().trimmed());
    }

    return data;
}
