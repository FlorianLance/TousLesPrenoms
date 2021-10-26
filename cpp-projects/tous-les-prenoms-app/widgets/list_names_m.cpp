
#include "list_names_m.hpp"

// Qt
#include <QColor>

using namespace tool;
using namespace tool::ui;


int ListNamesM::rowCount(const QModelIndex &) const{

    if(nData == nullptr){
        return 0;
    }

    if(!initialized){
        return nData->namesState.size();
    }

    if(m_mode == Mode::Filtered){
        return nData->countFiltered;
    }else if(m_mode == Mode::Saved){
        return nData->countSaved;
    }else if(m_mode == Mode::Removed){
        return nData->countRemoved;
    }
}

QVariant ListNamesM::data(const QModelIndex &index, int role) const{

    if (!index.isValid() || (nData == nullptr) || !initialized){
        return QVariant();
    }

    if (role == Qt::DisplayRole){

        if(m_mode == Mode::Filtered){
            if(index.row() < static_cast<int>(nData->filteredNames.size())){
                return nData->filteredNames[index.row()].v.toString();
            }
        }else if(m_mode == Mode::Saved){
            if(index.row() < static_cast<int>(nData->savedNames.size())){
                return nData->savedNames[index.row()].v.toString();
            }
        }else if(m_mode == Mode::Removed){
            if(index.row() < static_cast<int>(nData->removedNames.size())){
                return nData->removedNames[index.row()].v.toString();
            }
        }
        return "";

    }else if (role == Qt::BackgroundRole){

        if(m_mode == Mode::Filtered){
            if(index.row() < static_cast<int>(nData->filteredNames.size())){
                return dSettings->genderRepartitionsBackgroundColors[nData->pData.infosPerName[nData->filteredNames[index.row()]].genderRepartition];
            }
        }else if(m_mode == Mode::Saved){            
            if(index.row() < static_cast<int>(nData->savedNames.size())){
                return dSettings->genderRepartitionsBackgroundColors[nData->pData.infosPerName[nData->savedNames[index.row()]].genderRepartition];
            }
        }else if(m_mode == Mode::Removed){            
            if(index.row() < static_cast<int>(nData->removedNames.size())){
                return dSettings->genderRepartitionsBackgroundColors[nData->pData.infosPerName[nData->removedNames[index.row()]].genderRepartition];
            }
        }
        return QColor(255,255,255);

    }else if (role == Qt::ForegroundRole){

        if(m_mode == Mode::Filtered){
            if(index.row() < static_cast<int>(nData->filteredNames.size())){
                return dSettings->genderRepartitionsForegroundColors[nData->pData.infosPerName[nData->filteredNames[index.row()]].genderRepartition];
            }
        }else if(m_mode == Mode::Saved){
            if(index.row() < static_cast<int>(nData->savedNames.size())){
                return dSettings->genderRepartitionsForegroundColors[nData->pData.infosPerName[nData->savedNames[index.row()]].genderRepartition];
            }
        }else if(m_mode == Mode::Removed){
            if(index.row() < static_cast<int>(nData->removedNames.size())){
                return dSettings->genderRepartitionsForegroundColors[nData->pData.infosPerName[nData->removedNames[index.row()]].genderRepartition];
            }
        }

        return QColor(255,255,255);
    }//else if (role == Qt::TextAlignmentRole){
//        if(index.column() == 0){
//            return Qt::AlignLeft;
//        }else if(index.column() == 1){
//            return Qt::AlignCenter;
//        }
//    }

    return QVariant();
}


