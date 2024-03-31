#include "snap_viewer.h"
#include <QTreeView>
#include <qabstractitemmodel.h>
#include <qabstractitemview.h>
#include <qboxlayout.h>
#include <qevent.h>
#include <qlocale.h>
#include <qnamespace.h>
#include <qtreeview.h>
#include "binaryninjaapi.h"
#include "ui/action.h"
#include "ui/uicontext.h"

void SnapNotif::init(){
    UIAction::registerAction("Display Snapshots");
    auto actions = UIActionHandler::globalActions();
    actions->bindAction("Display Snapshots", UIAction(SnapViewer::display));
}

SnapTree::SnapTree(BinaryViewRef bv, UIActionContext ctx)  : QTreeView() {
    m_ctx = ctx;
    m_bv = bv;
}

void SnapTree::mouseDoubleClickEvent(QMouseEvent* event){
    auto database = m_bv->GetFile()->GetDatabase();
    m_ctx.context->getCurrentActionHandler()->executeAction("Restart Binary Ninja and Reopen Files");
    close();
}

void SnapTree::keyPressEvent(QKeyEvent* event){
    if(event->key() == Qt::Key_Return){
        auto database = m_bv->GetFile()->GetDatabase();
        auto index = this->selectedIndexes()[0];
        // TODO Set the current snapshot to the given index
        m_ctx.context->getCurrentActionHandler()->executeAction("Restart Binary Ninja and Reopen Files");
        event->accept();
        close();
    }
}


void SnapViewer::display(const UIActionContext &action){
    auto context = action.context;
    auto bv = context->getCurrentViewFrame()->getCurrentBinaryView();
    auto instance = new SnapViewer(bv, action);
    instance->show();
    instance->raise();
    instance->activateWindow();
}

SnapModel::SnapModel(BinaryViewRef bv) : QAbstractItemModel(){
    // qDebug() << "snap model created";
    m_fields.push_back(SnapField("Current", "current"));
    m_fields.push_back(SnapField("Id", "id"));
    m_fields.push_back(SnapField("Has Content", "has_contents"));
    m_fields.push_back(SnapField("Name", "name"));
    qDebug() <<  bv->GetFile()->GetDatabase()->GetCurrentSnapshot()->GetId();
    for(auto snapshot : bv->GetFile()->GetDatabase()->GetSnapshots()){
        SnapEntry entry;
        entry["id"] = snapshot->GetId();
        entry["has_contents"] = snapshot->HasContents();
        entry["name"] = snapshot->GetName();
        if(snapshot->GetId() == bv->GetFile()->GetDatabase()->GetCurrentSnapshot()->GetId()){
            entry["current"] = std::string("*");
            }
        else{
        entry["current"] = std::string("");
        }
        m_entries.push_back(entry);
    }
}

QModelIndex SnapModel::parent(const QModelIndex &child) const {
    return QModelIndex();
}
QVariant SnapModel::data(const QModelIndex &index, int role) const {
    if(index.row() >= m_entries.size()){
        return {};
    }
    if(role == Qt::DisplayRole){
        auto row = index.row();
        auto col = index.column();
        auto key = m_fields[col].second;
        auto entry = m_entries[row];
        auto value = entry[key];
        if(value.index() == 0){
           return QString::number(std::get<0>(value));
        }
        return QString::fromStdString(std::get<1>(value));
    }
    return {};
}
QVariant SnapModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(section >= m_fields.size()){
        qDebug() << section;
        return QVariant();
    }
    if(role != Qt::DisplayRole){
        return QVariant();
    }
    if(orientation != Qt::Horizontal){
        return QVariant();
    }
    qDebug() << m_fields[section].first.c_str();
    return QString::fromStdString(m_fields[section].first);
}
int SnapModel::rowCount(const QModelIndex &parent) const  {
    if(parent.isValid()){
        return 0;
    }
    return m_entries.size();
}
int SnapModel::columnCount(const QModelIndex &parent) const {
    if(parent.isValid()){
        return 0;
    }
    return m_fields.size();
}
QModelIndex SnapModel::index(int row, int column, const QModelIndex &parent) const {
    if(parent.isValid()){
        return QModelIndex();
    }
    if(row >= m_entries.size()){
        return QModelIndex();
    }
    if(column >= m_fields.size()){
        return QModelIndex();
    }
    return createIndex(row, column);
}

SnapViewer::SnapViewer(BinaryViewRef bv, UIActionContext ctx) : QDialog() {
    m_ctx = ctx;
    m_bv = bv;
    auto match_view = new SnapTree(bv, ctx);
    auto model = new SnapModel(bv);
    match_view->setModel(model);
    match_view->setContextMenuPolicy(Qt::CustomContextMenu);
    match_view->setSelectionMode(QTreeView::ExtendedSelection);
    match_view->setRootIsDecorated(false);
    match_view->setSortingEnabled(true);
    match_view->sortByColumn(0, Qt::AscendingOrder);
    auto layout = new QVBoxLayout();
    layout->addWidget(match_view);
    setLayout(layout);
    setWindowTitle("Snapshot Viewer");
    resize(700, 350);
}