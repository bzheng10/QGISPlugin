/***************************************************************************
 qgisapp.cpp  -  description
 -------------------

          begin                : Sat Jun 22 2002
          copyright            : (C) 2002 by Gary E.Sherman
          email                : sherman at mrcc.com
          Romans 3:23=>Romans 6:23=>Romans 10:9,10=>Romans 12
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Modified to work as a plugin for SimCenter tools by Dr. Stevan Gavrilovic

#include <QObject>
#include <QAction>
#include <QApplication>
#include <QBitmap>
#include <QCheckBox>
#include <QClipboard>
#include <QColor>
#include <QCursor>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QEvent>
#include <QUrlQuery>
#include <QFile>
#include <QFileInfo>
#include <QImageWriter>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QLibrary>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPictureIO>
#include <QPixmap>
#include <QPoint>
#include <QPrinter>
#include <QProcess>
#include <QProgressBar>
#include <QProgressDialog>
#include <QRegExp>
#include <QRegExpValidator>
#include <QScreen>
#include <QShortcut>
#include <QSpinBox>
#include <QSplashScreen>
#include <QUrl>
#include <QRegularExpression>
#ifndef QT_NO_SSL
#include <QSslConfiguration>
#endif
#include <QStatusBar>
#include <QStringList>
#include <QSysInfo>
#include <QTcpSocket>
#include <QTextStream>
#include <QtGlobal>
#include <QThread>
#include <QTimer>
#include <QToolButton>
#include <QUuid>
#include <QVBoxLayout>
#include <QWhatsThis>
#include <QWidgetAction>
#include <mutex>
#include <QWindow>

#include "qgssettingsregistrycore.h"
#include "qgsnetworkaccessmanager.h"
#include "qgsrelationmanager.h"
#include "qgsapplication.h"
#include "qgslayerstylingwidget.h"
#include "qgsdevtoolspanelwidget.h"
#include "qgstaskmanager.h"
#include "qgsweakrelation.h"
#include "qgsziputils.h"
#include "qgsbrowserguimodel.h"
#include "qgsvectorlayerjoinbuffer.h"
#include "qgsgeometryvalidationservice.h"
#include "qgssourceselectproviderregistry.h"
#include "qgssourceselectprovider.h"
#include "qgsprovidermetadata.h"
#include "qgsfixattributedialog.h"
#include "qgsprojecttimesettings.h"
#include "qgsmaplayertemporalproperties.h"
#include "qgsmaplayerutils.h"
#include "qgsgeometrycollection.h"
#include "qgsmeshlayertemporalproperties.h"
#include "qgsvectorlayersavestyledialog.h"
#include "maptools/qgsappmaptools.h"
#include "qgsexpressioncontextutils.h"
#include "qgsprovidersublayerdetails.h"
#include "qgsproviderutils.h"
#include "qgsprovidersublayersdialog.h"
#include "qgsmaplayerfactory.h"
#include "qgsbrowserwidget.h"
#include "annotations/qgsannotationitempropertieswidget.h"
#include "qgsmaptoolmodifyannotation.h"
#include "qgsannotationlayer.h"
#include "qgsdockablewidgethelper.h"
#include "vertextool/qgsvertexeditor.h"

#include "qgsanalysis.h"
#include "qgsgeometrycheckregistry.h"

#include "options/qgscodeeditoroptions.h"
#include "options/qgsgpsdeviceoptions.h"
#include "options/qgscustomprojectionoptions.h"

#include "raster/qgsrasterelevationpropertieswidget.h"
#include "vector/qgsvectorelevationpropertieswidget.h"
#include "mesh/qgsmeshelevationpropertieswidget.h"
#include "elevation/qgselevationprofilewidget.h"

#ifdef HAVE_3D
#include "qgs3d.h"
#include "qgs3danimationsettings.h"
#include "qgs3danimationwidget.h"
#include "qgs3dmapcanvas.h"
#include "qgs3dmapsettings.h"
#include "qgscameracontroller.h"
#include "qgsflatterraingenerator.h"
#include "qgslayoutitem3dmap.h"
#include "processing/qgs3dalgorithms.h"
#include "qgs3dmaptoolmeasureline.h"
#include "qgs3dsymbolregistry.h"
#include "layout/qgslayout3dmapwidget.h"
#include "layout/qgslayoutviewrubberband.h"
#include "qgsvectorlayer3drendererwidget.h"
#include "qgsmeshlayer3drendererwidget.h"
#include "qgspointcloudlayer3drendererwidget.h"
#include "qgs3dapputils.h"
#include "qgs3doptions.h"
#include "qgsmapviewsmanager.h"
#include "qgs3dmapcanvaswidget.h"
#include "qgs3dviewsmanagerdialog.h"
#include "qgs3dutils.h"
#endif

#ifdef HAVE_GEOREFERENCER
#include "georeferencer/qgsgeorefmainwindow.h"
#endif

#include "qgsgui.h"
#include "qgsnative.h"
#include "qgsdatasourceselectdialog.h"

#ifdef HAVE_OPENCL
#include "qgsopenclutils.h"
#endif

#include <QNetworkReply>
#include <QNetworkProxy>
#include <QAuthenticator>

Q_GUI_EXPORT extern int qt_defaultDpiX();

//
// Mac OS X Includes
// Must include before GEOS 3 due to unqualified use of 'Point'
//
#ifdef Q_OS_MACX
#include <ApplicationServices/ApplicationServices.h>
#include "qgsmacnative.h"

// check macro breaks QItemDelegate
#ifdef check
#undef check
#endif
#endif

//
// QGIS Specific Includes
//

#include "qgscrashhandler.h"

#include "qgisapp.h"
#include "qgisappinterface.h"
#include "qgisappstylesheet.h"
#include "qgis.h"
#include "qgisplugin.h"
#include "qgsabout.h"
#include "qgsabstractmaptoolhandler.h"
#include "qgsalignrasterdialog.h"
#include "qgsappauthrequesthandler.h"
#include "qgsappbrowserproviders.h"
#include "qgsapplayertreeviewmenuprovider.h"
#include "qgsapplication.h"
#include "qgsappsslerrorhandler.h"
#include "qgsactionmanager.h"
#include "qgsannotationmanager.h"
#include "qgsannotationregistry.h"
#include "qgsattributetabledialog.h"
#include "qgsattributedialog.h"
#include "qgsauthmanager.h"
#include "qgsauthguiutils.h"
#ifndef QT_NO_SSL
#include "qgsauthcertutils.h"
#include "qgsauthsslerrorsdialog.h"
#endif
#include "qgsappscreenshots.h"
#include "qgsapplicationexitblockerinterface.h"
#include "qgsbookmarks.h"
#include "qgsbookmarkeditordialog.h"
#include "qgsbrowserdockwidget.h"
#include "qgsadvanceddigitizingdockwidget.h"
#include "qgsclipboard.h"
#include "qgsconfigureshortcutsdialog.h"
#include "qgscoordinatetransform.h"
#include "qgscoordinateutils.h"
#include "qgscredentialdialog.h"
#include "qgscustomdrophandler.h"
#include "qgscustomprojectopenhandler.h"
#include "qgscustomization.h"
#include "qgscustomlayerorderwidget.h"
#include "qgsdataitemproviderregistry.h"
#include "qgsdataitemguiproviderregistry.h"
#include "qgsdatasourceuri.h"
#include "qgsdatumtransformdialog.h"
#include "qgsdoublespinbox.h"
#include "qgsdockwidget.h"
#include "qgsdxfexport.h"
#include "qgsdxfexportdialog.h"
#include "qgsdwgimportdialog.h"
#include "qgsdecorationtitle.h"
#include "qgsdecorationcopyright.h"
#include "qgsdecorationimage.h"
#include "qgsdecorationnortharrow.h"
#include "qgsdecorationscalebar.h"
#include "qgsdecorationgrid.h"
#include "qgsdecorationlayoutextent.h"
#include "qgsencodingfiledialog.h"
#include "qgserror.h"
#include "qgserrordialog.h"
#include "qgseventtracing.h"
#include "qgsexception.h"
#include "qgsexpressionselectiondialog.h"
#include "qgsfeature.h"
#include "qgsfieldcalculator.h"
#include "qgsfieldformatter.h"
#include "qgsfieldformatterregistry.h"
#include "qgsfileutils.h"
#include "qgsformannotation.h"
#include "qgsgeos.h"
#include "qgsguiutils.h"
#include "qgshtmlannotation.h"
#include "qgsprojectionselectiondialog.h"
#include "qgsgpsinformationwidget.h"
#include "qgsguivectorlayertools.h"
#include "qgslabelingwidget.h"
#include "qgsdiagramproperties.h"
#include "qgslayerdefinition.h"
#include "qgslayertree.h"
#include "qgslayertreegrouppropertieswidget.h"
#include "qgslayertreemapcanvasbridge.h"
#include "qgslayertreemodel.h"
#include "qgslayertreemodellegendnode.h"
#include "qgslayertreeregistrybridge.h"
#include "qgslayertreeutils.h"
#include "qgslayertreeview.h"
#include "qgslayertreeviewdefaultactions.h"
#include "qgslayertreeviewembeddedindicator.h"
#include "qgslayertreeviewfilterindicator.h"
#include "qgslayertreeviewlowaccuracyindicator.h"
#include "qgslayertreeviewmemoryindicator.h"
#include "qgslayertreeviewbadlayerindicator.h"
#include "qgslayertreeviewnonremovableindicator.h"
#include "qgslayertreeviewnotesindicator.h"
#include "qgslayertreeviewnocrsindicator.h"
#include "qgslayertreeviewtemporalindicator.h"
#include "qgslayertreeviewofflineindicator.h"
#include "qgsrasterpipe.h"
#include "qgslayout.h"
#include "qgslayoutatlas.h"
#include "qgslayoutcustomdrophandler.h"
#include "qgslayoutdesignerdialog.h"
#include "qgslayoutitemguiregistry.h"
#include "qgslayoutmanager.h"
#include "qgslayoutqptdrophandler.h"
#include "qgslayoutimagedrophandler.h"
#include "qgslayoutguiutils.h"
#include "qgslocatorwidget.h"
#include "qgslocator.h"
#include "qgsactionlocatorfilter.h"
#include "qgsactivelayerfeatureslocatorfilter.h"
#include "qgsalllayersfeatureslocatorfilter.h"
#include "qgsbookmarklocatorfilter.h"
#include "qgsexpressioncalculatorlocatorfilter.h"
#include "qgsgotolocatorfilter.h"
#include "qgslayertreelocatorfilter.h"
#include "qgslayoutlocatorfilter.h"
#include "qgsnominatimlocatorfilter.h"
#include "qgssettingslocatorfilter.h"
#include "qgsgeocoderlocatorfilter.h"
#include "qgsnominatimgeocoder.h"
#include "qgslogger.h"
#include "qgsmapcanvas.h"
#include "qgsmapcanvasdockwidget.h"
#include "qgsmapcanvassnappingutils.h"
#include "qgsmapcanvastracer.h"
#include "qgsmaplayer.h"
#include "qgsmaplayerstyleguiutils.h"
#include "qgsmapoverviewcanvas.h"
#include "qgsmapsettings.h"
#include "qgsmaptip.h"
#include "qgsmbtiles.h"
#include "qgsmenuheader.h"
#include "qgsmergeattributesdialog.h"
#include "qgsmessageviewer.h"
#include "qgsmessagebar.h"
#include "qgsmessagebaritem.h"
#include "qgsmeshlayer.h"
#include "qgsmeshlayerproperties.h"
#include "qgspointcloudlayer.h"
#include "qgsmemoryproviderutils.h"
#include "qgsmimedatautils.h"
#include "qgsmessagelog.h"
#include "qgsmultibandcolorrenderer.h"
#include "qgsnative.h"
#include "qgsnativealgorithms.h"
#include "qgsnewvectorlayerdialog.h"
#include "qgsnewmemorylayerdialog.h"
#include "qgsnewmeshlayerdialog.h"
#include "options/qgsoptions.h"
#include "qgspluginlayer.h"
#include "qgspluginlayerregistry.h"
#include "qgspluginmanager.h"
#include "qgspluginregistry.h"
#include "qgspointxy.h"
#include "qgspuzzlewidget.h"
#include "qgsruntimeprofiler.h"
#include "qgshandlebadlayers.h"
#include "qgsprintlayout.h"
#include "qgsprocessingregistry.h"
#include "qgsprojutils.h"
#include "qgsproject.h"
#include "qgsprojectlayergroupdialog.h"
#include "qgsprojectproperties.h"
#include "qgsprojectstorage.h"
#include "qgsprojectstorageguiprovider.h"
#include "qgsprojectstorageguiregistry.h"
#include "qgsprojectstorageregistry.h"
#include "qgsproviderregistry.h"
#include "qgsproviderguiregistry.h"
#include "qgspythonrunner.h"
#include "qgsproxyprogresstask.h"
#include "qgsquerybuilder.h"
#include "qgspointcloudquerybuilder.h"
#include "qgsrastercalcdialog.h"
#include "qgsmeshcalculatordialog.h"
#include "qgsrasterfilewriter.h"
#include "qgsrasterfilewritertask.h"
#include "qgsrasteriterator.h"
#include "qgsrasterlayer.h"
#include "qgsrasterlayerproperties.h"
#include "qgsrasternuller.h"
#include "qgsbrightnesscontrastfilter.h"
#include "qgsrasterrenderer.h"
#include "qgsrasterlayersaveasdialog.h"
#include "qgsrasterprojector.h"
#include "qgsreadwritecontext.h"
#include "qgsrectangle.h"
#include "qgsreport.h"
#include "qgsscalevisibilitydialog.h"
#include "qgsgroupwmsdatadialog.h"
#include "qgsselectbyformdialog.h"
#include "qgsshortcutsmanager.h"
#include "qgssinglebandgrayrenderer.h"
#include "qgssnappingwidget.h"
#include "qgsstatisticalsummarydockwidget.h"
#include "qgsstatusbar.h"
#include "qgsstatusbarcoordinateswidget.h"
#include "qgsstatusbarmagnifierwidget.h"
#include "qgsstatusbarscalewidget.h"
#include "qgsstyle.h"
#include "qgssubsetstringeditorproviderregistry.h"
#include "qgssubsetstringeditorprovider.h"
#include "qgssubsetstringeditorinterface.h"
#include "qgssvgannotation.h"
#include "qgstaskmanager.h"
#include "qgstaskmanagerwidget.h"
#include "qgssymbolselectordialog.h"
#include "qgstextannotation.h"
#include "qgsundowidget.h"
#include "qgsuserinputwidget.h"
#include "qgsvectordataprovider.h"
#include "qgsvectorfilewriter.h"
#include "qgsvectorlayer.h"
#include "qgsvectorlayerproperties.h"
#include "qgsvectorlayerdigitizingproperties.h"
#include "qgsvectortilelayer.h"
#include "qgsvectortilelayerproperties.h"
#include "qgspointcloudlayerproperties.h"
#include "qgsmapthemes.h"
#include "qgsmessagelogviewer.h"
#include "qgsdataitem.h"
#include "qgsmaplayeractionregistry.h"
#include "qgswelcomepage.h"
#include "qgsversioninfo.h"
#include "qgslegendfilterbutton.h"
#include "qgsvirtuallayerdefinition.h"
#include "qgsvirtuallayerdefinitionutils.h"
#include "qgstransaction.h"
#include "qgstransactiongroup.h"
#include "qgsvectorlayerjoininfo.h"
#include "qgsvectorlayerutils.h"
#include "qgshelp.h"
#include "qgsvectorfilewritertask.h"
#include "qgsmapsavedialog.h"
#include "qgsmaprenderertask.h"
#include "qgsmapdecoration.h"
#include "qgsnewnamedialog.h"
#include "qgsgui.h"
#include "qgsdatasourcemanagerdialog.h"
#include "qgsappwindowmanager.h"
#include "qgsvaliditycheckregistry.h"
#include "qgsappcoordinateoperationhandlers.h"
#include "qgsprojectviewsettings.h"
#include "qgscoordinateformatter.h"
#include "qgslocaldefaultsettings.h"
#include "qgsbearingnumericformat.h"
#include "qgsprojectdisplaysettings.h"
#include "qgstemporalcontrollerdockwidget.h"
#include "qgsnetworklogger.h"
#include "qgsuserprofilemanager.h"
#include "qgsuserprofile.h"
#include "qgsnetworkloggerwidgetfactory.h"
#include "devtools/profiler/qgsprofilerwidgetfactory.h"
#include "qgsabstractdatabaseproviderconnection.h"
#include "qgszipitem.h"

#include "browser/qgsinbuiltdataitemproviders.h"

#include "qgssublayersdialog.h"
#include "ogr/qgsvectorlayersaveasdialog.h"
#include "qgsannotationitemguiregistry.h"
#include "annotations/qgsannotationlayerproperties.h"
#include "qgscreateannotationitemmaptool.h"

#include "pointcloud/qgspointcloudelevationpropertieswidget.h"
#include "pointcloud/qgspointcloudlayerstylewidget.h"
#include "project/qgsprojectelevationsettingswidget.h"

#include "qgsmaptoolsdigitizingtechniquemanager.h"
#include "qgsmaptoolshaperegistry.h"
#include "qgsmaptoolshapecircularstringradius.h"
#include "qgsmaptoolshapecircle2points.h"
#include "qgsmaptoolshapecircle3points.h"
#include "qgsmaptoolshapecircle3tangents.h"
#include "qgsmaptoolshapecircle2tangentspoint.h"
#include "qgsmaptoolshapecirclecenterpoint.h"
#include "qgsmaptoolshapeellipsecenter2points.h"
#include "qgsmaptoolshapeellipsecenterpoint.h"
#include "qgsmaptoolshapeellipseextent.h"
#include "qgsmaptoolshapeellipsefoci.h"
#include "qgsmaptoolshaperectanglecenter.h"
#include "qgsmaptoolshaperectangleextent.h"
#include "qgsmaptoolshaperectangle3points.h"
#include "qgsmaptoolshaperegularpolygon2points.h"
#include "qgsmaptoolshaperegularpolygoncenterpoint.h"
#include "qgsmaptoolshaperegularpolygoncentercorner.h"

#ifdef ENABLE_MODELTEST
#include "modeltest.h"
#endif

//
// GDAL/OGR includes
//
#include <ogr_api.h>
#include <gdal_version.h>
#include <proj.h>

#ifdef HAVE_PDAL_QGIS
#include <pdal/pdal.hpp>
#endif

//
// Other includes
//
#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <iomanip>
#include <list>
#include <memory>
#include <vector>

#include "qgsmeasuretool.h"
#include "qgsmapcanvasannotationitem.h"
#include "qgsmaptoolpan.h"
#include "qgsmaptoolidentifyaction.h"
#include "qgsmaptoolpinlabels.h"
#include "qgsmaptoolmeasureangle.h"
#include "qgsmaptoolmeasurebearing.h"
#include "qgsmaptoolrotatepointsymbols.h"
#include "qgsmaptooldigitizefeature.h"
#include "qgsmaptooloffsetpointsymbol.h"
#include "vertextool/qgsvertextool.h"
#include "qgsmaptooleditmeshframe.h"

#include "qgsgeometryvalidationmodel.h"
#include "qgsgeometryvalidationdock.h"
#include "qgslayoutvaliditychecks.h"

// Editor widgets
#include "qgseditorwidgetregistry.h"
//
// Conditional Includes
//
#ifdef HAVE_PGCONFIG
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include <pg_config.h>
#else
#define PG_VERSION "unknown"
#endif

#include <sqlite3.h>

#ifdef HAVE_SPATIALITE
extern "C"
{
#include <spatialite.h>
}
#include "qgsnewspatialitelayerdialog.h"
#endif

#include "qgsnewgeopackagelayerdialog.h"

#ifdef WITH_BINDINGS
#include "qgspythonutils.h"
#endif

#ifndef Q_OS_WIN
#include <dlfcn.h>
#else
#include <shellapi.h>
#include <dbghelp.h>
#endif

class QTreeWidgetItem;
class QgsUserProfileManager;
class QgsUserProfile;

/**
 * Set the application title bar text
 */
static void setTitleBarText_( QWidget &qgisApp )
{
  QString caption;
  if ( QgsProject::instance()->title().isEmpty() )
  {
    if ( QgsProject::instance()->fileName().isEmpty() )
    {
      // new project
      caption = QgisApp::tr( "Untitled Project" );
    }
    else
    {
      caption = QgsProject::instance()->baseName();
    }
  }
  else
  {
    caption = QgsProject::instance()->title();
  }
  if ( !caption.isEmpty() )
  {
    caption += QStringLiteral( " %1 " ).arg( QChar( 0x2014 ) );
  }
  if ( QgsProject::instance()->isDirty() )
    caption.prepend( '*' );

  caption += QgisApp::tr( "QGIS" );

  if ( Qgis::version().endsWith( QLatin1String( "Master" ) ) )
  {
    caption += QStringLiteral( " %1" ).arg( Qgis::devVersion() );
  }

  if ( QgisApp::instance()->userProfileManager()->allProfiles().count() > 1 )
  {
    // add current profile (if it's not the default one)
    QgsUserProfile *profile = QgisApp::instance()->userProfileManager()->userProfile();
    if ( profile->name() != QLatin1String( "default" ) )
      caption += QStringLiteral( " [%1]" ).arg( profile->name() );
  }

  qgisApp.setWindowTitle( caption );
}

/**
 * Creator function for output viewer
*/
static QgsMessageOutput *messageOutputViewer_()
{
  if ( QThread::currentThread() == qApp->thread() )
    return new QgsMessageViewer( QgisApp::instance() );
  else
    return new QgsMessageOutputConsole();
}

static void customSrsValidation_( QgsCoordinateReferenceSystem &srs )
{
  const QgsOptions::UnknownLayerCrsBehavior mode = QgsSettings().enumValue( QStringLiteral( "/projections/unknownCrsBehavior" ), QgsOptions::UnknownLayerCrsBehavior::NoAction, QgsSettings::App );
  switch ( mode )
  {
    case QgsOptions::UnknownLayerCrsBehavior::NoAction:
      return;

    case QgsOptions::UnknownLayerCrsBehavior::UseDefaultCrs:
      srs.createFromOgcWmsCrs( QgsSettings().value( QStringLiteral( "Projections/layerDefaultCrs" ), geoEpsgCrsAuthId() ).toString() );
      break;

    case QgsOptions::UnknownLayerCrsBehavior::PromptUserForCrs:
    case QgsOptions::UnknownLayerCrsBehavior::UseProjectCrs:
      // can't take any action immediately for these -- we may be in a background thread
      break;
  }

  if ( QThread::currentThread() != QApplication::instance()->thread() )
  {
    // Running in a background thread -- we can't queue this connection, because
    // srs is a reference and may be deleted before the queued slot is called.
    // We also can't do ANY gui related stuff here. Best we can do is log
    // a warning and move on...
    QgsMessageLog::logMessage( QObject::tr( "Layer has unknown CRS" ) );
  }
  else
  {
    QgisApp::instance()->emitCustomCrsValidation( srs );
  }
}

void QgisApp::emitCustomCrsValidation( QgsCoordinateReferenceSystem &srs )
{
  emit customCrsValidation( srs );
}

void QgisApp::layerTreeViewDoubleClicked( const QModelIndex &index )
{
  Q_UNUSED( index )
  QgsSettings settings;
  switch ( settings.value( QStringLiteral( "qgis/legendDoubleClickAction" ), 0 ).toInt() )
  {
    case 0:
    {
      //show properties
      if ( mLayerTreeView )
      {
        // if it's a legend node, open symbol editor directly
        if ( QgsSymbolLegendNode *node = qobject_cast<QgsSymbolLegendNode *>( mLayerTreeView->currentLegendNode() ) )
        {
          const QgsSymbol *originalSymbol = node->symbol();
          if ( !originalSymbol )
            return;

          std::unique_ptr< QgsSymbol > symbol( originalSymbol->clone() );
          QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( node->layerNode()->layer() );
          QgsSymbolSelectorDialog dlg( symbol.get(), QgsStyle::defaultStyle(), vlayer, this );
          QgsSymbolWidgetContext context;
          context.setMapCanvas( mMapCanvas );
          context.setMessageBar( mInfoBar );
          dlg.setContext( context );
          if ( dlg.exec() )
          {
            node->setSymbol( symbol.release() );
          }

          return;
        }
      }
      QgisApp::instance()->layerProperties();
      break;
    }
    case 1:
    {
      QgsSettings settings;
      QgsAttributeTableFilterModel::FilterMode initialMode = settings.enumValue( QStringLiteral( "qgis/attributeTableBehavior" ),  QgsAttributeTableFilterModel::ShowAll );
      QgisApp::instance()->attributeTable( initialMode );
      break;
    }
    case 2:
      mapStyleDock( true );
      break;
    default:
      break;
  }
}

void QgisApp::onActiveLayerChanged( QgsMapLayer *layer )
{
  if ( mBlockActiveLayerChanged )
    return;

  const QList< QgsMapCanvas * > canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
    canvas->setCurrentLayer( layer );

  if ( mUndoWidget )
  {
    if ( layer )
    {
      mUndoWidget->setUndoStack( layer->undoStack() );
    }
    else
    {
      mUndoWidget->unsetStack();
    }
    updateUndoActions();
  }

  emit activeLayerChanged( layer );
}

void QgisApp::vectorLayerStyleLoaded( QgsVectorLayer *vl, QgsMapLayer::StyleCategories categories )
{
  if ( vl && vl->isValid( ) )
  {

    // Check broken dependencies in forms
    if ( categories.testFlag( QgsMapLayer::StyleCategory::Forms ) )
    {
      resolveVectorLayerDependencies( vl );
    }

    // Check broken relations and try to restore them
    if ( categories.testFlag( QgsMapLayer::StyleCategory::Relations ) )
    {
      resolveVectorLayerWeakRelations( vl );
    }

  }
}

void QgisApp::toggleEventTracing()
{
  QgsSettings settings;
  if ( !settings.value( QStringLiteral( "qgis/enableEventTracing" ), false ).toBool() )
  {
    // make sure the setting is available in Options > Advanced
    if ( !settings.contains( QStringLiteral( "qgis/enableEventTracing" ) ) )
      settings.setValue( QStringLiteral( "qgis/enableEventTracing" ), false );

    messageBar()->pushWarning( tr( "Event Tracing" ), tr( "Tracing is not enabled. Look for \"enableEventTracing\" in Options > Advanced." ) );
    return;
  }

  if ( !QgsEventTracing::isTracingEnabled() )
  {
    messageBar()->pushSuccess( tr( "Event Tracing" ), tr( "Tracing started." ) );
    QgsEventTracing::startTracing();
  }
  else
  {
    QgsEventTracing::stopTracing();
    QString fileName = QFileDialog::getSaveFileName( this, tr( "Save Event Trace..." ), QString(), tr( "Event Traces (*.json)" ) );
    if ( !fileName.isEmpty() )
      QgsEventTracing::writeTrace( fileName );
  }
}

#ifdef HAVE_GEOREFERENCER
void QgisApp::showGeoreferencer()
{
  if ( !mGeoreferencer )
    mGeoreferencer = new QgsGeoreferencerMainWindow( this );
  mGeoreferencer->show();
  mGeoreferencer->setFocus();
}
#endif

void QgisApp::annotationItemTypeAdded( int id )
{
  if ( QgsGui::annotationItemGuiRegistry()->itemMetadata( id )->flags() & Qgis::AnnotationItemGuiFlag::FlagNoCreationTools )
    return;

  QString name = QgsGui::annotationItemGuiRegistry()->itemMetadata( id )->visibleName();
  QString groupId = QgsGui::annotationItemGuiRegistry()->itemMetadata( id )->groupId();
  QToolButton *groupButton = nullptr;
  if ( !groupId.isEmpty() )
  {
    // find existing group toolbutton and submenu, or create new ones if this is the first time the group has been encountered
    const QgsAnnotationItemGuiGroup &group = QgsGui::annotationItemGuiRegistry()->itemGroup( groupId );
    QIcon groupIcon = group.icon.isNull() ? QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddBasicShape.svg" ) ) : group.icon;
    QString groupText = tr( "Create %1" ).arg( group.name );
    if ( mAnnotationItemGroupToolButtons.contains( groupId ) )
    {
      groupButton = mAnnotationItemGroupToolButtons.value( groupId );
    }
    else
    {
      QToolButton *groupToolButton = new QToolButton( mAnnotationsToolBar );
      groupToolButton->setIcon( groupIcon );
      groupToolButton->setCheckable( true );
      groupToolButton->setPopupMode( QToolButton::InstantPopup );
      groupToolButton->setAutoRaise( true );
      groupToolButton->setToolButtonStyle( Qt::ToolButtonIconOnly );
      groupToolButton->setToolTip( groupText );
      mAnnotationsToolBar->insertWidget( mAnnotationsItemInsertBefore, groupToolButton );
      mAnnotationItemGroupToolButtons.insert( groupId, groupToolButton );
      groupButton = groupToolButton;
    }
  }

  // update UI for new item type
  QAction *action = new QAction( tr( "Create %1" ).arg( name ), this );
  action->setToolTip( tr( "Create %1" ).arg( name ) );
  action->setCheckable( true );
  action->setData( id );
  action->setIcon( QgsGui::annotationItemGuiRegistry()->itemMetadata( id )->creationIcon() );

  mMapToolGroup->addAction( action );

  if ( groupButton )
    groupButton->addAction( action );
  else
  {
    mAnnotationsToolBar->insertAction( mAnnotationsItemInsertBefore, action );
  }

  connect( action, &QAction::toggled, this, [this, action, id]( bool checked )
  {
    if ( !checked )
      return;

    QgsCreateAnnotationItemMapToolInterface *tool = QgsGui::annotationItemGuiRegistry()->itemMetadata( id )->createMapTool( mMapCanvas, mAdvancedDigitizingDockWidget );
    tool->mapTool()->setAction( action );
    mMapCanvas->setMapTool( tool->mapTool() );
    if ( qobject_cast< QgsMapToolCapture * >( tool->mapTool() ) )
    {
      mDigitizingTechniqueManager->enableDigitizingTechniqueActions( checked, action );
    }

    connect( tool->mapTool(), &QgsMapTool::deactivated, tool->mapTool(), &QObject::deleteLater );
    connect( tool->handler(), &QgsCreateAnnotationItemMapToolHandler::itemCreated, this, [ = ]
    {
      QgsAnnotationItem *item = tool->handler()->takeCreatedItem();
      QgsAnnotationLayer *targetLayer = qobject_cast< QgsAnnotationLayer * >( activeLayer() );
      if ( !targetLayer )
        targetLayer = QgsProject::instance()->mainAnnotationLayer();

      const QString itemId = targetLayer->addItem( item );
      // automatically select item in layer styling panel
      mMapStyleWidget->setAnnotationItem( targetLayer, itemId );
      mMapStylingDock->setUserVisible( true );
      mMapStyleWidget->focusDefaultWidget();

      QgsProject::instance()->setDirty( true );

      // TODO -- possibly automatically deactivate the tool now?
    } );
  } );
}

/*
 * This function contains forced validation of CRS used in QGIS.
 * There are 4 options depending on the settings:
 * - ask for CRS using projection selecter
 * - use project's CRS
 * - use predefined global CRS
 * - take no action (leave as unknown CRS)
 */
void QgisApp::validateCrs( QgsCoordinateReferenceSystem &srs )
{
  static QString sAuthId = QString();

  const QgsOptions::UnknownLayerCrsBehavior mode = QgsSettings().enumValue( QStringLiteral( "/projections/unknownCrsBehavior" ), QgsOptions::UnknownLayerCrsBehavior::NoAction, QgsSettings::App );
  switch ( mode )
  {
    case QgsOptions::UnknownLayerCrsBehavior::NoAction:
      break;

    case QgsOptions::UnknownLayerCrsBehavior::UseDefaultCrs:
    {
      srs.createFromOgcWmsCrs( QgsSettings().value( QStringLiteral( "Projections/layerDefaultCrs" ), geoEpsgCrsAuthId() ).toString() );
      sAuthId = srs.authid();
      visibleMessageBar()->pushMessage( tr( "CRS was undefined" ), tr( "defaulting to CRS %1" ).arg( srs.userFriendlyIdentifier() ), Qgis::MessageLevel::Warning );
      break;
    }

    case QgsOptions::UnknownLayerCrsBehavior::PromptUserForCrs:
    {
      // \note this class is not a descendent of QWidget so we can't pass
      // it in the ctor of the layer projection selector

      static bool opening = false;
      if ( opening )
        break;
      opening = true;

      QgsProjectionSelectionDialog *mySelector = new QgsProjectionSelectionDialog();
      const QString validationHint = srs.validationHint();
      if ( !validationHint.isEmpty() )
        mySelector->setMessage( validationHint );
      else
        mySelector->showNoCrsForLayerMessage();

      if ( sAuthId.isNull() )
        sAuthId = QgsProject::instance()->crs().authid();

      QgsCoordinateReferenceSystem defaultCrs( sAuthId );
      if ( defaultCrs.isValid() )
      {
        mySelector->setCrs( defaultCrs );
      }

      QgsTemporaryCursorRestoreOverride cursorOverride;

      if ( mySelector->exec() )
      {
        QgsDebugMsgLevel( "Layer srs set from dialog: " + QString::number( mySelector->crs().srsid() ), 2 );
        srs = mySelector->crs();
        sAuthId = srs.authid();
      }

      delete mySelector;
      opening = false;
      break;
    }

    case QgsOptions::UnknownLayerCrsBehavior::UseProjectCrs:
    {
      // XXX TODO: Change project to store selected CS as 'projectCRS' not 'selectedWkt'
      srs = QgsProject::instance()->crs();
      sAuthId = srs.authid();
      QgsDebugMsgLevel( "Layer srs set from project: " + sAuthId, 2 );
      visibleMessageBar()->pushMessage( tr( "CRS was undefined" ), tr( "defaulting to project CRS %1" ).arg( srs.userFriendlyIdentifier() ), Qgis::MessageLevel::Warning );
      break;
    }
  }
}


static bool cmpByText_( QAction *a, QAction *b )
{
  return QString::localeAwareCompare( a->text(), b->text() ) < 0;
}


QgisApp *QgisApp::sInstance = nullptr;

// constructor starts here
QgisApp::QgisApp( QMainWindow *mainWindow, bool restorePlugins, bool skipVersionCheck, const QString &rootProfileLocation, const QString &activeProfile, QWidget *parent, Qt::WindowFlags fl)
    : QMainWindow( parent, fl )
{
  if ( sInstance )
  {
    QMessageBox::critical(
      this,
      tr( "Multiple Instances of QgisApp" ),
      tr( "Multiple instances of QGIS application object detected.\nPlease contact the developers.\n" ) );
    abort();
  }

  sInstance = this;
  QgsRuntimeProfiler *profiler = QgsApplication::profiler();

  // SG add delete splash screen
  // QColor splashTextColor = Qgis::releaseName() == QLatin1String( "Master" ) ? QColor( 93, 153, 51 ) : Qt::black;

  startProfile( tr( "Create user profile manager" ) );
  mUserProfileManager = new QgsUserProfileManager( QString(), this );
  mUserProfileManager->setRootLocation( rootProfileLocation );
  mUserProfileManager->setActiveUserProfile( activeProfile );
  mUserProfileManager->setNewProfileNotificationEnabled( true );
  connect( mUserProfileManager, &QgsUserProfileManager::profilesChanged, this, &QgisApp::refreshProfileMenu );
  endProfile();

  // start the network logger early, we want all requests logged!
  startProfile( tr( "Create network logger" ) );
  mNetworkLogger = new QgsNetworkLogger( QgsNetworkAccessManager::instance(), this );
  endProfile();

  // load GUI: actions, menus, toolbars
  startProfile( tr( "Setting up UI" ) );

  setupActions();
  setupMenus(mainWindow);
  setupToolBars();

  // because mActionToggleMapOnly can hide the menu (thereby disabling menu actions),
  //  we attach the following actions to the MainWindow too (to be able to come back)
  this->addAction( mActionToggleFullScreen );
  this->addAction( mActionTogglePanelsVisibility );
  this->addAction( mActionToggleMapOnly );
  endProfile();

  setDockOptions( dockOptions() | QMainWindow::GroupedDragging );

  //////////

  // SG add
  //  startProfile( tr( "Checking user database" ) );
  //  //mSplash->showMessage( tr( "Checking database" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor );  // SG add delete splash screen
  //  qApp->processEvents();
  //  // Do this early on before anyone else opens it and prevents us copying it
  //  QString dbError;
  //  if ( !QgsApplication::createDatabase( &dbError ) )
  //  {
  //    QMessageBox::critical( this, tr( "Private qgis.db" ), dbError );
  //  }
  //  endProfile();

  // Create the themes folder for the user
  startProfile( tr( "Creating theme folder" ) );
  QgsApplication::createThemeFolder();
  endProfile();

  //mSplash->showMessage( tr( "Reading settings" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor );  // SG add delete splash screen
  qApp->processEvents();

  //mSplash->showMessage( tr( "Setting up the GUI" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor );  // SG add delete splash screen
  qApp->processEvents();

  QgsApplication::initQgis();
  if ( !QgsApplication::authManager()->isDisabled() )
  {
    // Most of the auth initialization is now done inside initQgis, no need to profile here
    masterPasswordSetup();
  }

  QgsSettings settings;


  startProfile( tr( "Building style sheet" ) );
  // set up stylesheet builder and apply saved or default style options
  mStyleSheetBuilder = new QgisAppStyleSheet( this );
  connect( mStyleSheetBuilder, &QgisAppStyleSheet::appStyleSheetChanged,
           this, &QgisApp::setAppStyleSheet );
  endProfile();

  //  QWidget *centralWidget = this->centralWidget();
  //  QGridLayout *centralLayout = new QGridLayout( centralWidget );
  //  centralWidget->setLayout( centralLayout );
  //  centralLayout->setContentsMargins( 0, 0, 0, 0 );

  // SG Change
  auto centralWidget = new QWidget();
  centralWidget->setObjectName(QString::fromUtf8("centralwidget"));
  this->setCentralWidget(centralWidget);

  QGridLayout *centralLayout = new QGridLayout();
  centralWidget->setLayout( centralLayout );
  centralLayout->setContentsMargins( 0, 0, 0, 0 );

  // "theMapCanvas" used to find this canonical instance later
  startProfile( tr( "Creating map canvas" ) );
  mMapCanvas = new QgsMapCanvas( centralWidget );
  mMapCanvas->setObjectName( QStringLiteral( "theMapCanvas" ) );

  // before anything, let's freeze canvas redraws
  QgsCanvasRefreshBlocker refreshBlocker;

  connect( mMapCanvas, &QgsMapCanvas::messageEmitted, this, &QgisApp::displayMessage );

  if ( settings.value( QStringLiteral( "qgis/main_canvas_preview_jobs" ) ).isNull() )
  {
    // So that it appears in advanced settings
    settings.setValue( QStringLiteral( "qgis/main_canvas_preview_jobs" ), true );
  }
  mMapCanvas->setPreviewJobsEnabled( settings.value( QStringLiteral( "qgis/main_canvas_preview_jobs" ), true ).toBool() );

  // set canvas color right away
  int myRed = settings.value( QStringLiteral( "qgis/default_canvas_color_red" ), 255 ).toInt();
  int myGreen = settings.value( QStringLiteral( "qgis/default_canvas_color_green" ), 255 ).toInt();
  int myBlue = settings.value( QStringLiteral( "qgis/default_canvas_color_blue" ), 255 ).toInt();
  mMapCanvas->setCanvasColor( QColor( myRed, myGreen, myBlue ) );

  // set project linked to main canvas
  mMapCanvas->setProject( QgsProject::instance() );
  endProfile();

  // what type of project to auto-open
  mProjOpen = settings.value( QStringLiteral( "qgis/projOpenAtLaunch" ), 0 ).toInt();

  // a bar to warn the user with non-blocking messages
  startProfile( tr( "Message bar" ) );
  mInfoBar = new QgsMessageBar( centralWidget );
  mInfoBar->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
  centralLayout->addWidget( mInfoBar, 0, 0, 1, 1 );
  endProfile();

  // SG change
  //  startProfile( tr( "Welcome page" ) );
  //  mWelcomePage = new QgsWelcomePage( skipVersionCheck );
  //  connect( mWelcomePage, &QgsWelcomePage::projectRemoved, this, [ this ]( int row )
  //  {
  //    mRecentProjects.removeAt( row );
  //    saveRecentProjects();
  //    updateRecentProjectPaths();
  //  } );
  //  connect( mWelcomePage, &QgsWelcomePage::projectPinned, this, [ this ]( int row )
  //  {
  //    mRecentProjects.at( row ).pin = true;
  //    saveRecentProjects();
  //    updateRecentProjectPaths();
  //  } );
  //  connect( mWelcomePage, &QgsWelcomePage::projectUnpinned, this, [ this ]( int row )
  //  {
  //    mRecentProjects.at( row ).pin = false;
  //    saveRecentProjects();
  //    updateRecentProjectPaths();
  //  } );
  //  endProfile();

  mCentralContainer = new QStackedWidget;
  mCentralContainer->insertWidget( 0, mMapCanvas );
  //  mCentralContainer->insertWidget( 1, mWelcomePage );   // SG change

  centralLayout->addWidget( mCentralContainer, 0, 0, 2, 1 );
  mInfoBar->raise();

  connect( mMapCanvas, &QgsMapCanvas::layersChanged, this, &QgisApp::showMapCanvas );

  //  mCentralContainer->setCurrentIndex( mProjOpen ? 0 : 1 ); // SG change

  startProfile( tr( "User input dock" ) );
  // User Input Dock Widget
  mUserInputDockWidget = new QgsUserInputWidget( mMapCanvas );
  mUserInputDockWidget->setObjectName( QStringLiteral( "UserInputDockWidget" ) );
  mUserInputDockWidget->setAnchorWidget( mMapCanvas );
  mUserInputDockWidget->setAnchorWidgetPoint( QgsFloatingWidget::TopRight );
  mUserInputDockWidget->setAnchorPoint( QgsFloatingWidget::TopRight );

  endProfile();

  //set the focus to the map canvas
  mMapCanvas->setFocus();

  startProfile( tr( "Layer tree" ) );
  mLayerTreeView = new QgsLayerTreeView( this );
  mLayerTreeView->setObjectName( QStringLiteral( "theLayerTreeView" ) ); // "theLayerTreeView" used to find this canonical instance later
  endProfile();

  // create undo widget
  startProfile( tr( "Undo dock" ) );
  mUndoDock = new QgsDockWidget( tr( "Undo/Redo" ), this );
  QShortcut *showUndoDock = new QShortcut( QKeySequence( tr( "Ctrl+5" ) ), this );
  connect( showUndoDock, &QShortcut::activated, mUndoDock, &QgsDockWidget::toggleUserVisible );
  showUndoDock->setObjectName( QStringLiteral( "ShowUndoPanel" ) );
  showUndoDock->setWhatsThis( tr( "Show Undo/Redo Panel" ) );

  mUndoWidget = new QgsUndoWidget( mUndoDock, mMapCanvas );
  mUndoWidget->setObjectName( QStringLiteral( "Undo" ) );
  mUndoDock->setWidget( mUndoWidget );
  mUndoDock->setObjectName( QStringLiteral( "undo/redo dock" ) );
  endProfile();

  // Advanced Digitizing dock
  startProfile( tr( "Advanced digitize panel" ) );
  mAdvancedDigitizingDockWidget = new QgsAdvancedDigitizingDockWidget( mMapCanvas, this );
  mAdvancedDigitizingDockWidget->setWindowTitle( tr( "Advanced Digitizing" ) );
  mAdvancedDigitizingDockWidget->setObjectName( QStringLiteral( "AdvancedDigitizingTools" ) );

  QShortcut *showAdvancedDigitizingDock = new QShortcut( QKeySequence( tr( "Ctrl+4" ) ), this );
  connect( showAdvancedDigitizingDock, &QShortcut::activated, mAdvancedDigitizingDockWidget, &QgsDockWidget::toggleUserVisible );
  showAdvancedDigitizingDock->setObjectName( QStringLiteral( "ShowAdvancedDigitizingPanel" ) );
  showAdvancedDigitizingDock->setWhatsThis( tr( "Show Advanced Digitizing Panel" ) );

  endProfile();

  // Statistical Summary dock
  startProfile( tr( "Statistics dock" ) );
  mStatisticalSummaryDockWidget = new QgsStatisticalSummaryDockWidget( this );
  mStatisticalSummaryDockWidget->setObjectName( QStringLiteral( "StatisticalSummaryDockWidget" ) );

  QShortcut *showStatsDock = new QShortcut( QKeySequence( tr( "Ctrl+6" ) ), this );
  connect( showStatsDock, &QShortcut::activated, mStatisticalSummaryDockWidget, &QgsDockWidget::toggleUserVisible );
  showStatsDock->setObjectName( QStringLiteral( "ShowStatisticsPanel" ) );
  showStatsDock->setWhatsThis( tr( "Show Statistics Panel" ) );

  endProfile();

  // Bookmarks dock
  startProfile( tr( "Bookmarks widget" ) );
  mBookMarksDockWidget = new QgsBookmarks( this );
  mBookMarksDockWidget->setObjectName( QStringLiteral( "BookmarksDockWidget" ) );

  QShortcut *showBookmarksDock = new QShortcut( QKeySequence( tr( "Ctrl+7" ) ), this );
  connect( showBookmarksDock, &QShortcut::activated, mBookMarksDockWidget, &QgsDockWidget::toggleUserVisible );
  showBookmarksDock->setObjectName( QStringLiteral( "ShowBookmarksPanel" ) );
  showBookmarksDock->setWhatsThis( tr( "Show Bookmarks Panel" ) );
  mBookMarksDockWidget->setToggleVisibilityAction( mActionShowBookmarkManager );

  connect( mActionShowBookmarks, &QAction::triggered, this, [ = ] { showBookmarks(); } );

  endProfile();

  startProfile( tr( "Snapping utilities" ) );
  mSnappingUtils = new QgsMapCanvasSnappingUtils( mMapCanvas, this );
  mMapCanvas->setSnappingUtils( mSnappingUtils );
  connect( QgsProject::instance(), &QgsProject::snappingConfigChanged, mSnappingUtils, &QgsSnappingUtils::setConfig );

  endProfile();

  functionProfile( &QgisApp::createMenus, this, QStringLiteral( "Create menus" ) );
  functionProfile( &QgisApp::createActions, this, QStringLiteral( "Create actions" ) );
  functionProfile( &QgisApp::createActionGroups, this, QStringLiteral( "Create action group" ) );

  // create map tools
  mVertexEditorDock = new QgsVertexEditor( mMapCanvas );
  mVertexEditorDock->setObjectName( QStringLiteral( "VertexEditor" ) );
  addDockWidget( Qt::LeftDockWidgetArea, mVertexEditorDock );
  mVertexEditorDock->hide();

  mMapTools = std::make_unique< QgsAppMapTools >( mMapCanvas, mAdvancedDigitizingDockWidget );
  mDigitizingTechniqueManager = new QgsMapToolsDigitizingTechniqueManager( this );

  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeCircularStringRadiusMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeCircle2PointsMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeCircle3PointsMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeCircle3TangentsMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeCircle2TangentsPointMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeCircleCenterPointMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeEllipseCenter2PointsMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeEllipseCenterPointMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeEllipseExtentMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeEllipseFociMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeRectangleCenterMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeRectangleExtentMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeRectangle3PointsMetadata( QgsMapToolShapeRectangle3PointsMetadata::CreateMode::Distance ) );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeRectangle3PointsMetadata( QgsMapToolShapeRectangle3PointsMetadata::CreateMode::Projected ) );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeRegularPolygon2PointsMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeRegularPolygonCenterPointMetadata() );
  QgsGui::mapToolShapeRegistry()->addMapTool( new QgsMapToolShapeRegularPolygonCenterCornerMetadata() );


  functionProfile( &QgisApp::createToolBars, this, QStringLiteral( "Toolbars" ) );
  functionProfile( &QgisApp::createStatusBar, this, QStringLiteral( "Status bar" ) );
  functionProfile( &QgisApp::setupCanvasTools, this, QStringLiteral( "Create canvas tools" ) );

  applyDefaultSettingsToCanvas( mMapCanvas );

  functionProfile( &QgisApp::initLayerTreeView, this, QStringLiteral( "Initialize layer tree view" ) );
  functionProfile( &QgisApp::createOverview, this, QStringLiteral( "Create overview" ) );
  functionProfile( &QgisApp::createMapTips, this, QStringLiteral( "Create map tips" ) );
  functionProfile( &QgisApp::createDecorations, this, QStringLiteral( "Create decorations" ) );
  functionProfile( &QgisApp::readSettings, this, QStringLiteral( "Read settings" ) );
  functionProfile( &QgisApp::updateProjectFromTemplates, this, QStringLiteral( "Update project from templates" ) );
  functionProfile( &QgisApp::legendLayerSelectionChanged, this, QStringLiteral( "Legend layer selection changed" ) );
  functionProfile( &QgisApp::init3D, this, QStringLiteral( "Initialize 3D support" ) );
  functionProfile( &QgisApp::initNativeProcessing, this, QStringLiteral( "Initialize native processing" ) );
  functionProfile( &QgisApp::initLayouts, this, QStringLiteral( "Initialize layouts support" ) );

  startProfile( tr( "Geometry validation" ) );

  mGeometryValidationService = std::make_unique<QgsGeometryValidationService>( QgsProject::instance() );
  mGeometryValidationService->setMessageBar( mInfoBar );
  mGeometryValidationDock = new QgsGeometryValidationDock( tr( "Geometry Validation" ), mMapCanvas, this );
  mGeometryValidationDock->hide();
  mGeometryValidationModel = new QgsGeometryValidationModel( mGeometryValidationService.get(), mGeometryValidationDock );
  connect( this, &QgisApp::activeLayerChanged, mGeometryValidationModel, [this]( QgsMapLayer * layer )
  {
    mGeometryValidationModel->setCurrentLayer( qobject_cast<QgsVectorLayer *>( layer ) );
  } );
  mGeometryValidationDock->setGeometryValidationModel( mGeometryValidationModel );
  mGeometryValidationDock->setGeometryValidationService( mGeometryValidationService.get() );
  endProfile();

  QgsApplication::annotationRegistry()->addAnnotationType( QgsAnnotationMetadata( QStringLiteral( "FormAnnotationItem" ), &QgsFormAnnotation::create ) );
  connect( QgsProject::instance()->annotationManager(), &QgsAnnotationManager::annotationAdded, this, &QgisApp::annotationCreated );

  mSaveRollbackInProgress = false;

  QString templateDirName = settings.value( QStringLiteral( "qgis/projectTemplateDir" ),
                            QString( QgsApplication::qgisSettingsDirPath() + "project_templates" ) ).toString();
  if ( !QFileInfo::exists( templateDirName ) )
  {
    // create default template directory
    if ( !QDir().mkdir( QgsApplication::qgisSettingsDirPath() + "project_templates" ) )
      templateDirName.clear();
  }
  if ( !templateDirName.isEmpty() ) // template directory exists, so watch it!
  {
    QFileSystemWatcher *projectsTemplateWatcher = new QFileSystemWatcher( this );
    projectsTemplateWatcher->addPath( templateDirName );
    connect( projectsTemplateWatcher, &QFileSystemWatcher::directoryChanged, this, [this] { updateProjectFromTemplates(); } );
  }

  // initialize the plugin manager
  startProfile( tr( "Plugin manager" ) );
  mPluginManager = new QgsPluginManager( this, restorePlugins );
  endProfile();

  addDockWidget( Qt::LeftDockWidgetArea, mUndoDock );
  mUndoDock->hide();

  startProfile( tr( "Layer style dock" ) );
  mMapStylingDock = new QgsDockWidget( this );
  mMapStylingDock->setWindowTitle( tr( "Layer Styling" ) );
  mMapStylingDock->setObjectName( QStringLiteral( "LayerStyling" ) );
  QShortcut *showStylingDock = new QShortcut( QKeySequence( tr( "Ctrl+3" ) ), this );
  connect( showStylingDock, &QShortcut::activated, mMapStylingDock, &QgsDockWidget::toggleUserVisible );
  showStylingDock->setObjectName( QStringLiteral( "ShowLayerStylingPanel" ) );
  showStylingDock->setWhatsThis( tr( "Show Style Panel" ) );

  mMapStyleWidget = new QgsLayerStylingWidget( mMapCanvas, mInfoBar, mMapLayerPanelFactories );
  mMapStylingDock->setWidget( mMapStyleWidget );
  connect( mMapStyleWidget, &QgsLayerStylingWidget::styleChanged, this, &QgisApp::updateLabelToolButtons );
  connect( mMapStyleWidget, &QgsLayerStylingWidget::layerStyleChanged, this, [ = ]( const QString & styleName )
  {
    if ( !QgsMapLayerStyleManager::isDefault( styleName ) && !styleName.isEmpty() )
    {
      mMapStylingDock->setWindowTitle( tr( "Layer Styling (%1)" ).arg( styleName ) );
    }
    else
    {
      mMapStylingDock->setWindowTitle( tr( "Layer Styling" ) );
    }
  } );
  connect( mMapStylingDock, &QDockWidget::visibilityChanged, mActionStyleDock, &QAction::setChecked );

  addDockWidget( Qt::RightDockWidgetArea, mMapStylingDock );
  mMapStylingDock->hide();
  endProfile();

  startProfile( tr( "Developer tools dock" ) );
  mDevToolsDock = new QgsDockWidget( this );
  mDevToolsDock->setWindowTitle( tr( "Debugging/Development Tools" ) );
  mDevToolsDock->setObjectName( QStringLiteral( "DevTools" ) );
  QShortcut *showDevToolsDock = new QShortcut( QKeySequence( tr( "F12" ) ), this );
  connect( showDevToolsDock, &QShortcut::activated, mDevToolsDock, &QgsDockWidget::toggleUserVisible );
  showDevToolsDock->setObjectName( QStringLiteral( "ShowDevToolsPanel" ) );
  showDevToolsDock->setWhatsThis( tr( "Show Debugging/Development Tools" ) );

  mDevToolsWidget = new QgsDevToolsPanelWidget( mDevToolFactories );
  mDevToolsDock->setWidget( mDevToolsWidget );
//  connect( mDevToolsDock, &QDockWidget::visibilityChanged, mActionStyleDock, &QAction::setChecked );

  addDockWidget( Qt::RightDockWidgetArea, mDevToolsDock );
  mDevToolsDock->hide();
  endProfile();

  startProfile( tr( "Snapping dialog" ) );
  mSnappingDialog = new QgsSnappingWidget( QgsProject::instance(), mMapCanvas, this );
  connect( mSnappingDialog, &QgsSnappingWidget::snappingConfigChanged, QgsProject::instance(), [ = ] { QgsProject::instance()->setSnappingConfig( mSnappingDialog->config() ); } );
  QString mainSnappingWidgetMode = QgsSettings().value( QStringLiteral( "/qgis/mainSnappingWidgetMode" ), "dialog" ).toString();
  if ( mainSnappingWidgetMode == QLatin1String( "dock" ) )
  {
    QgsDockWidget *dock = new QgsDockWidget( tr( "Snapping and Digitizing Options" ), QgisApp::instance() );
    dock->setAllowedAreas( Qt::AllDockWidgetAreas );
    dock->setWidget( mSnappingDialog );
    dock->setObjectName( QStringLiteral( "Snapping and Digitizing Options" ) );
    addDockWidget( Qt::LeftDockWidgetArea, dock );
    mSnappingDialogContainer = dock;
    dock->hide();
  }
  else
  {
    QDialog *dialog = new QDialog( this, Qt::Tool );
    dialog->setObjectName( QStringLiteral( "snappingSettings" ) );
    dialog->setWindowTitle( tr( "Project Snapping Settings" ) );
    QgsGui::enableAutoGeometryRestore( dialog );
    QVBoxLayout *layout = new QVBoxLayout( dialog );
    layout->addWidget( mSnappingDialog );
    layout->setContentsMargins( 0, 0, 0, 0 );
    mSnappingDialogContainer = dialog;
  }
  endProfile();

  mBrowserModel = new QgsBrowserGuiModel( this );
  mBrowserWidget = new QgsBrowserDockWidget( tr( "Browser" ), mBrowserModel, this );
  mBrowserWidget->setObjectName( QStringLiteral( "Browser" ) );
  mBrowserWidget->setMessageBar( mInfoBar );

  mTemporalControllerWidget = new QgsTemporalControllerDockWidget( tr( "Temporal Controller" ), this );
  mTemporalControllerWidget->setObjectName( QStringLiteral( "Temporal Controller" ) );
  addDockWidget( Qt::TopDockWidgetArea, mTemporalControllerWidget );
  mTemporalControllerWidget->hide();
  mTemporalControllerWidget->setToggleVisibilityAction( mActionTemporalController );

  mMapCanvas->setTemporalController( mTemporalControllerWidget->temporalController() );
  mTemporalControllerWidget->setMapCanvas( mMapCanvas );

  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsAppDirectoryItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsAppFileItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsProjectHomeItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsProjectItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsFavoritesItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsLayerItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsBookmarksItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsFieldsItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsFieldItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsFieldDomainItemGuiProvider() );
  QgsGui::dataItemGuiProviderRegistry()->addProvider( new QgsDatabaseItemGuiProvider() );

  QShortcut *showBrowserDock = new QShortcut( QKeySequence( tr( "Ctrl+2" ) ), this );
  connect( showBrowserDock, &QShortcut::activated, mBrowserWidget, &QgsDockWidget::toggleUserVisible );
  showBrowserDock->setObjectName( QStringLiteral( "ShowBrowserPanel" ) );
  showBrowserDock->setWhatsThis( tr( "Show Browser Panel" ) );

  addDockWidget( Qt::LeftDockWidgetArea, mBrowserWidget );
  mBrowserWidget->hide();
  // Only connect the first widget: the model is shared, there is no need to refresh multiple times.
  connect( this, &QgisApp::connectionsChanged, mBrowserWidget, [ = ]
  {
    if ( !mBlockBrowser1Refresh && !mBlockBrowser2Refresh )
      mBrowserWidget->refresh();
  } );
  connect( mBrowserWidget, &QgsBrowserDockWidget::connectionsChanged, this, [ = ]
  {
    mBlockBrowser1Refresh++;
    emit connectionsChanged();
    mBlockBrowser1Refresh--;
  } );
  connect( mBrowserWidget, &QgsBrowserDockWidget::openFile, this, &QgisApp::openFile );
  connect( mBrowserWidget, &QgsBrowserDockWidget::handleDropUriList, this, &QgisApp::handleDropUriList );

  mBrowserWidget2 = new QgsBrowserDockWidget( tr( "Browser (2)" ), mBrowserModel, this );
  mBrowserWidget2->setObjectName( QStringLiteral( "Browser2" ) );
  addDockWidget( Qt::LeftDockWidgetArea, mBrowserWidget2 );
  mBrowserWidget2->hide();
  connect( mBrowserWidget2, &QgsBrowserDockWidget::connectionsChanged, this, [ = ]
  {
    mBlockBrowser2Refresh++;
    emit connectionsChanged();
    mBlockBrowser2Refresh--;
  } );
  connect( mBrowserWidget2, &QgsBrowserDockWidget::openFile, this, &QgisApp::openFile );
  connect( mBrowserWidget2, &QgsBrowserDockWidget::handleDropUriList, this, &QgisApp::handleDropUriList );

  addDockWidget( Qt::LeftDockWidgetArea, mAdvancedDigitizingDockWidget );
  mAdvancedDigitizingDockWidget->hide();

  addDockWidget( Qt::LeftDockWidgetArea, mStatisticalSummaryDockWidget );
  mStatisticalSummaryDockWidget->hide();

  addDockWidget( Qt::LeftDockWidgetArea, mBookMarksDockWidget );
  mBookMarksDockWidget->hide();

  // create the GPS tool on starting QGIS - this is like the browser
  mpGpsWidget = new QgsGpsInformationWidget( mMapCanvas );
  QgsPanelWidgetStack *gpsStack = new QgsPanelWidgetStack();
  gpsStack->setMainPanel( mpGpsWidget );
  mpGpsWidget->setDockMode( true );
  //create the dock widget
  mpGpsDock = new QgsDockWidget( tr( "GPS Information" ), this );

  QShortcut *showGpsDock = new QShortcut( QKeySequence( tr( "Ctrl+0" ) ), this );
  connect( showGpsDock, &QShortcut::activated, mpGpsDock, &QgsDockWidget::toggleUserVisible );
  showGpsDock->setObjectName( QStringLiteral( "ShowGpsPanel" ) );
  showGpsDock->setWhatsThis( tr( "Show GPS Information Panel" ) );

  mpGpsDock->setObjectName( QStringLiteral( "GPSInformation" ) );
  mpGpsDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  addDockWidget( Qt::LeftDockWidgetArea, mpGpsDock );
  // add to the Panel submenu
  // now add our widget to the dock - ownership of the widget is passed to the dock
  mpGpsDock->setWidget( gpsStack );
  mpGpsDock->hide();

  mLastMapToolMessage = nullptr;

  mLogViewer = new QgsMessageLogViewer( this );

  mLogDock = new QgsDockWidget( tr( "Log Messages" ), this );
  mLogDock->setObjectName( QStringLiteral( "MessageLog" ) );
  mLogDock->setAllowedAreas( Qt::AllDockWidgetAreas );
  addDockWidget( Qt::BottomDockWidgetArea, mLogDock );
  mLogDock->setWidget( mLogViewer );
  mLogDock->hide();
  connect( mMessageButton, &QAbstractButton::toggled, mLogDock, &QgsDockWidget::setUserVisible );
  connect( mLogDock, &QgsDockWidget::visibilityChanged, mMessageButton, &QAbstractButton::setChecked );
  connect( QgsApplication::messageLog(), static_cast < void ( QgsMessageLog::* )( bool ) >( &QgsMessageLog::messageReceived ), this, &QgisApp::toggleLogMessageIcon );
  connect( mMessageButton, &QAbstractButton::toggled, this, &QgisApp::toggleLogMessageIcon );
  mVectorLayerTools = new QgsGuiVectorLayerTools();

  // Init the editor widget types
  QgsGui::editorWidgetRegistry()->initEditors( mMapCanvas, mInfoBar );

  mInternalClipboard = new QgsClipboard; // create clipboard
  connect( mInternalClipboard, &QgsClipboard::changed, this, &QgisApp::clipboardChanged );
  mQgisInterface = new QgisAppInterface( this ); // create the interface

#ifdef Q_OS_MAC
  // action for Window menu (create before generating WindowTitleChange event))
  mWindowAction = new QAction( this );
  connect( mWindowAction, SIGNAL( triggered() ), this, SLOT( activate() ) );

  // add this window to Window menu
  addWindow( mWindowAction );
#endif

  registerMapLayerPropertiesFactory( new QgsVectorLayerDigitizingPropertiesFactory( this ) );
  registerMapLayerPropertiesFactory( new QgsPointCloudRendererWidgetFactory( this ) );
#ifdef HAVE_3D
  registerMapLayerPropertiesFactory( new QgsVectorLayer3DRendererWidgetFactory( this ) );
  registerMapLayerPropertiesFactory( new QgsMeshLayer3DRendererWidgetFactory( this ) );
  registerMapLayerPropertiesFactory( new QgsPointCloudLayer3DRendererWidgetFactory( this ) );
#endif
  registerMapLayerPropertiesFactory( new QgsPointCloudElevationPropertiesWidgetFactory( this ) );
  registerMapLayerPropertiesFactory( new QgsRasterElevationPropertiesWidgetFactory( this ) );
  registerMapLayerPropertiesFactory( new QgsVectorElevationPropertiesWidgetFactory( this ) );
  registerMapLayerPropertiesFactory( new QgsMeshElevationPropertiesWidgetFactory( this ) );
  registerMapLayerPropertiesFactory( new QgsAnnotationItemPropertiesWidgetFactory( this ) );
  registerMapLayerPropertiesFactory( new QgsLayerTreeGroupPropertiesWidgetFactory( this ) );

  registerProjectPropertiesWidgetFactory( new QgsProjectElevationSettingsWidgetFactory( this ) );

  activateDeactivateLayerRelatedActions( nullptr ); // after members were created

  connect( QgsGui::mapLayerActionRegistry(), &QgsMapLayerActionRegistry::changed, this, &QgisApp::refreshActionFeatureAction );

  // set application's caption
  QString caption = tr( "QGIS - %1 ('%2')" ).arg( Qgis::version(), Qgis::releaseName() );
  setWindowTitle( caption );

  // QgsMessageLog::logMessage( tr( "QGIS starting…" ), QString(), Qgis::MessageLevel::Info );

  connect( QgsProject::instance(), &QgsProject::isDirtyChanged, this, [ = ] { setTitleBarText_( *this ); } );

  // set QGIS specific srs validation
  connect( this, &QgisApp::customCrsValidation,
           this, &QgisApp::validateCrs );
  QgsCoordinateReferenceSystem::setCustomCrsValidation( customSrsValidation_ );

  // set graphical message output
  QgsMessageOutput::setMessageOutputCreator( messageOutputViewer_ );

  // set graphical credential requester
  new QgsCredentialDialog( this );

  mLocatorWidget->setMapCanvas( mMapCanvas );
  connect( mLocatorWidget, &QgsLocatorWidget::configTriggered, this, [ = ] { showOptionsDialog( this, QStringLiteral( "mOptionsLocatorSettings" ) ); } );

  qApp->processEvents();

  // load providers
  // mSplash->showMessage( tr( "Checking provider plugins" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor );  // SG add delete splash screen
  qApp->processEvents();

  // Setup QgsNetworkAccessManager (this needs to happen after authentication, for proxy settings)
  namSetup();


#ifdef HAVE_OPENCL
  // Setup the default OpenCL programs source path, this my be overridden later by main.cpp startup
  QgsOpenClUtils::setSourcePath( QDir( QgsApplication::pkgDataPath() ).absoluteFilePath( QStringLiteral( "resources/opencl_programs" ) ) );
#endif


  QgsApplication::dataItemProviderRegistry()->addProvider( new QgsBookmarksDataItemProvider() );
  registerCustomDropHandler( new QgsBookmarkDropHandler() );
  QgsApplication::dataItemProviderRegistry()->addProvider( new QgsQlrDataItemProvider() );
  registerCustomDropHandler( new QgsQlrDropHandler() );
  QgsApplication::dataItemProviderRegistry()->addProvider( new QgsQptDataItemProvider() );
  registerCustomDropHandler( new QgsQptDropHandler() );
  QgsApplication::dataItemProviderRegistry()->addProvider( new QgsStyleXmlDataItemProvider() );
  registerCustomDropHandler( new QgsStyleXmlDropHandler() );
  QgsApplication::dataItemProviderRegistry()->addProvider( new QgsHtmlDataItemProvider() );

  auto skipBadLayers = false; // SG add

  // set handler for missing layers (will be owned by QgsProject)
  if ( !skipBadLayers )
  {
    QgsDebugMsgLevel( QStringLiteral( "Creating bad layers handler" ), 2 );
    mAppBadLayersHandler = new QgsHandleBadLayersHandler();
    QgsProject::instance()->setBadLayerHandler( mAppBadLayersHandler );
  }

  //mSplash->showMessage( tr( "Starting Python" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor );  // SG add delete splash screen
  qApp->processEvents();
  //  loadPythonSupport(); // SG add

#ifdef WITH_BINDINGS
  QgsApplication::dataItemProviderRegistry()->addProvider( new QgsPyDataItemProvider() );
  registerCustomDropHandler( new QgsPyDropHandler() );
#endif

  QgsApplication::dataItemProviderRegistry()->addProvider( new QgsProjectDataItemProvider() );

  // now when all data item providers are registered, customize both browsers
  QgsCustomization::instance()->updateBrowserWidget( mBrowserWidget );
  QgsCustomization::instance()->updateBrowserWidget( mBrowserWidget2 );


  // populate annotation toolbar with initial items...
  const QList< int > itemMetadataIds = QgsGui::annotationItemGuiRegistry()->itemMetadataIds();
  for ( int id : itemMetadataIds )
  {
    annotationItemTypeAdded( id );
  }
  //..and listen out for new item types
  connect( QgsGui::annotationItemGuiRegistry(), &QgsAnnotationItemGuiRegistry::typeAdded, this, &QgisApp::annotationItemTypeAdded );


  // Create the plugin registry and load plugins
  // load any plugins that were running in the last session
  // mSplash->showMessage( tr( "Restoring loaded plugins" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor ); // SG add delete splash screen
  qApp->processEvents();
  QgsPluginRegistry::instance()->setQgisInterface( mQgisInterface );

  if ( restorePlugins )
  {
    // Restoring of plugins can be disabled with --noplugins command line option
    // because some plugins may cause QGIS to crash during startup
    QgsPluginRegistry::instance()->restoreSessionPlugins( QgsApplication::pluginPath() );

    // Also restore plugins from user specified plugin directories
    QString myPaths = settings.value( QStringLiteral( "plugins/searchPathsForPlugins" ), "" ).toString();
    if ( !myPaths.isEmpty() )
    {
      QStringList myPathList = myPaths.split( '|' );
      QgsPluginRegistry::instance()->restoreSessionPlugins( myPathList );
    }
  }

#ifdef WITH_BINDINGS
  if ( mPythonUtils && mPythonUtils->isEnabled() )
  {
    startProfile( tr( "Plugin installer" ) );
    // initialize the plugin installer to start fetching repositories in background
    QgsPythonRunner::run( QStringLiteral( "import pyplugin_installer" ) );
    QgsPythonRunner::run( QStringLiteral( "pyplugin_installer.initPluginInstaller()" ) );
    // enable Python in the Plugin Manager and pass the PythonUtils to it
    mPluginManager->setPythonUtils( mPythonUtils );
    // add Python Console options
    initPythonConsoleOptions();
    endProfile();
  }
  else if ( mActionShowPythonDialog )
#endif
  {
    // python is disabled so get rid of the action for python console
    // and installing plugin from ZUIP
    delete mActionShowPythonDialog;
    mActionShowPythonDialog = nullptr;
  }

  // Update recent project list (as possible custom project storages are now registered by plugins)
  // mSplash->showMessage( tr( "Updating recent project paths" ), Qt::AlignHCenter | Qt::AlignBottom );  // SG add delete splash screen
  qApp->processEvents();
  startProfile( tr( "Update recent project paths" ) );
  updateRecentProjectPaths();
  // mWelcomePage->setRecentProjects( mRecentProjects ); // SG Change
  endProfile();

  // Set icon size of toolbars
  if ( settings.contains( QStringLiteral( "/qgis/iconSize" ) ) )
  {
    int size = settings.value( QStringLiteral( "/qgis/iconSize" ) ).toInt();
    if ( size < 16 )
      size = QGIS_ICON_SIZE;
    setIconSizes( size );
  }
  else
  {
    // first run, guess a good icon size
    int size = chooseReasonableDefaultIconSize();
    settings.setValue( QStringLiteral( "/qgis/iconSize" ), size );
    setIconSizes( size );
  }

  QgsApplication::validityCheckRegistry()->addCheck( new QgsLayoutScaleBarValidityCheck() );
  QgsApplication::validityCheckRegistry()->addCheck( new QgsLayoutNorthArrowValidityCheck() );
  QgsApplication::validityCheckRegistry()->addCheck( new QgsLayoutOverviewValidityCheck() );
  QgsApplication::validityCheckRegistry()->addCheck( new QgsLayoutPictureSourceValidityCheck() );

  // mSplash->showMessage( tr( "Initializing file filters" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor );  // SG add delete splash screen
  qApp->processEvents();

  // now build vector and raster file filters
  mVectorFileFilter = QgsProviderRegistry::instance()->fileVectorFilters();
  mRasterFileFilter = QgsProviderRegistry::instance()->fileRasterFilters();

#if 0
  // Set the background color for toolbox and overview as they default to
  // white instead of the window color
  QPalette myPalette = toolBox->palette();
  myPalette.setColor( QPalette::Button, myPalette.window().color() );
  toolBox->setPalette( myPalette );
  //do the same for legend control
  myPalette = toolBox->palette();
  myPalette.setColor( QPalette::Button, myPalette.window().color() );
  mMapLegend->setPalette( myPalette );
  //and for overview control this is done in createOverView method
#endif
  // Do this last in the ctor to ensure that all members are instantiated properly
  setupConnections();
  //
  // Please make sure this is the last thing the ctor does so that we can ensure the
  // widgets are all initialized before trying to restore their state.
  //
  // mSplash->showMessage( tr( "Restoring window state" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor );  // SG add delete splash screen
  qApp->processEvents();
  startProfile( tr( "Restore window state" ) );
  restoreWindowState();
  endProfile();

  // do main window customization - after window state has been restored, before the window is shown
  startProfile( tr( "Update customization on main window" ) );
  QgsCustomization::instance()->updateMainWindow( mToolbarMenu, mPanelMenu );
  endProfile();

  // mSplash->showMessage( tr( "Populate saved styles" ), Qt::AlignHCenter | Qt::AlignBottom, splashTextColor ); // SG add delete splash screen
  startProfile( tr( "Populate saved styles" ) );
  QgsStyle::defaultStyle();
  endProfile();

  mStatusBar->showMessage( tr(  "SimCenter + QGIS"  )); // SG add simcenter

  QgsMessageLog::logMessage( QgsApplication::showSettings(), QString(), Qgis::MessageLevel::Info );

  //QgsMessageLog::logMessage( tr( "QGIS Ready!" ), QString(), Qgis::MessageLevel::Info );

  mMapTipsVisible = false;
  // This turns on the map tip if they where active in the last session
  if ( settings.value( QStringLiteral( "qgis/enableMapTips" ), false ).toBool() )
  {
    toggleMapTips( true );
  }

  mPythonMacrosEnabled = false;

  // setup drag drop
  setAcceptDrops( true );

  mFullScreenMode = false;
  mPrevScreenModeMaximized = false;
  // SG change
  //  startProfile( tr( "Show main window" ) );
  //  show();
  //  qApp->processEvents();
  //  endProfile();

  QgsGui::setWindowManager( new QgsAppWindowManager() );

  mMapCanvas->clearExtentHistory(); // reset zoomnext/zoomlast

  QShortcut *zoomInShortCut = new QShortcut( QKeySequence( tr( "Ctrl++" ) ), this );
  connect( zoomInShortCut, &QShortcut::activated, mMapCanvas, &QgsMapCanvas::zoomIn );
  zoomInShortCut->setObjectName( QStringLiteral( "ZoomInToCanvas" ) );
  zoomInShortCut->setWhatsThis( tr( "Zoom in to canvas" ) );
  zoomInShortCut->setProperty( "Icon", QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomIn.svg" ) ) );

  QShortcut *zoomShortCut2 = new QShortcut( QKeySequence( tr( "Ctrl+=" ) ), this );
  connect( zoomShortCut2, &QShortcut::activated, mMapCanvas, &QgsMapCanvas::zoomIn );
  zoomShortCut2->setObjectName( QStringLiteral( "ZoomInToCanvas2" ) );
  zoomShortCut2->setWhatsThis( tr( "Zoom in to canvas (secondary)" ) );
  zoomShortCut2->setProperty( "Icon", QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomIn.svg" ) ) );

  QShortcut *zoomOutShortCut = new QShortcut( QKeySequence( tr( "Ctrl+-" ) ), this );
  connect( zoomOutShortCut, &QShortcut::activated, mMapCanvas, &QgsMapCanvas::zoomOut );
  zoomOutShortCut->setObjectName( QStringLiteral( "ZoomOutOfCanvas" ) );
  zoomOutShortCut->setWhatsThis( tr( "Zoom out of canvas" ) );
  zoomOutShortCut->setProperty( "Icon", QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomOut.svg" ) ) );

  //also make ctrl+alt+= a shortcut to switch to zoom in map tool
  QShortcut *zoomInToolShortCut = new QShortcut( QKeySequence( tr( "Ctrl+Alt+=" ) ), this );
  connect( zoomInToolShortCut, &QShortcut::activated, this, &QgisApp::zoomIn );
  zoomInToolShortCut->setObjectName( QStringLiteral( "ZoomIn2" ) );
  zoomInToolShortCut->setWhatsThis( tr( "Zoom in (secondary)" ) );
  zoomInToolShortCut->setProperty( "Icon", QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomIn.svg" ) ) );

  QShortcut *shortcutTracing = new QShortcut( QKeySequence( tr( "Ctrl+Shift+." ) ), this );
  shortcutTracing->setObjectName( QStringLiteral( "ToggleTracing" ) );
  connect( shortcutTracing, &QShortcut::activated, this, &QgisApp::toggleEventTracing );

  if ( ! QTouchDevice::devices().isEmpty() )
  {
    //add reacting to long click in touch
    grabGesture( Qt::TapAndHoldGesture );
  }

  connect( QgsApplication::taskManager(), &QgsTaskManager::statusChanged, this, &QgisApp::onTaskCompleteShowNotify );

  QgsGui::nativePlatformInterface()->initializeMainWindow( windowHandle(),
      QgsApplication::applicationName(),
      QgsApplication::organizationName(),
      Qgis::version() );
  connect( QgsGui::nativePlatformInterface(), &QgsNative::usbStorageNotification, mBrowserModel, &QgsBrowserModel::refreshDrives );

  // setup application progress reports from task manager
  connect( QgsApplication::taskManager(), &QgsTaskManager::taskAdded, this, []
  {
    QgsGui::nativePlatformInterface()->showUndefinedApplicationProgress();
  } );
  connect( QgsApplication::taskManager(), &QgsTaskManager::finalTaskProgressChanged, this, []( double val )
  {
    QgsGui::nativePlatformInterface()->setApplicationProgress( val );
  } );
  connect( QgsApplication::taskManager(), &QgsTaskManager::allTasksFinished, this, []
  {
    QgsGui::nativePlatformInterface()->hideApplicationProgress();
  } );
  connect( QgsApplication::taskManager(), &QgsTaskManager::countActiveTasksChanged, this, []( int count )
  {
    QgsGui::nativePlatformInterface()->setApplicationBadgeCount( count );
  } );

  ( void )new QgsAppMissingGridHandler( this );

  // supposedly all actions have been added, now register them to the shortcut manager
  QgsGui::shortcutsManager()->registerAllChildren( this );
  // QgsGui::shortcutsManager()->registerAllChildren( mSnappingWidget ); // SG Change

  // register additional action
  auto registerShortcuts = [ = ]( const QString & sequence, const QString & objectName, const QString & whatsThis )
  {
    QShortcut *sc = new QShortcut( QKeySequence( sequence ), this );
    sc->setContext( Qt::ApplicationShortcut );
    sc->setObjectName( objectName );
    sc->setWhatsThis( whatsThis );
    QgsGui::shortcutsManager()->registerShortcut( sc, sequence );
  };
  registerShortcuts( QStringLiteral( "Ctrl+Alt+{" ), QStringLiteral( "mAttributeTableFirstEditedFeature" ), tr( "Edit first feature in attribute table" ) );
  registerShortcuts( QStringLiteral( "Ctrl+Alt+[" ), QStringLiteral( "mAttributeTablePreviousEditedFeature" ), tr( "Edit previous feature in attribute table" ) );
  registerShortcuts( QStringLiteral( "Ctrl+Alt+]" ), QStringLiteral( "mAttributeTableNextEditedFeature" ), tr( "Edit next feature in attribute table" ) );
  registerShortcuts( QStringLiteral( "Ctrl+Alt+}" ), QStringLiteral( "mAttributeTableLastEditedFeature" ), tr( "Edit last feature in attribute table" ) );

  QgsGui::providerGuiRegistry()->registerGuis( this );

  setupLayoutManagerConnections();

#ifdef HAVE_3D
  connect( QgsProject::instance()->viewsManager(), &QgsMapViewsManager::views3DListChanged, this, &QgisApp::views3DMenuAboutToShow );
#endif

  setupDuplicateFeaturesAction();

  // support for project storage
  connect( mProjectFromStorageMenu, &QMenu::aboutToShow, this, [this] { populateProjectStorageMenu( mProjectFromStorageMenu, false ); } );
  connect( mProjectToStorageMenu, &QMenu::aboutToShow, this, [this] { populateProjectStorageMenu( mProjectToStorageMenu, true ); } );

  QList<QAction *> actions = mPanelMenu->actions();
  std::sort( actions.begin(), actions.end(), cmpByText_ );
  mPanelMenu->insertActions( nullptr, actions );

  mBearingNumericFormat.reset( QgsLocalDefaultSettings::bearingFormat() );

  mNetworkLoggerWidgetFactory.reset( std::make_unique< QgsNetworkLoggerWidgetFactory >( mNetworkLogger ) );

  // update windows
  qApp->processEvents();

  // notify user if authentication system is disabled
  //( void )QgsAuthGuiUtils::isDisabled( messageBar() ); // SG Change

  startProfile( tr( "New project" ) );
  fileNewBlank(); // prepare empty project, also skips any default templates from loading
  updateCrsStatusBar();
  endProfile();

  connect( qobject_cast< QgsMapToolModifyAnnotation * >( mMapTools->mapTool( QgsAppMapTools::AnnotationEdit ) ), &QgsMapToolModifyAnnotation::itemSelected,
           mMapStyleWidget, &QgsLayerStylingWidget::setAnnotationItem );
  connect( qobject_cast< QgsMapToolModifyAnnotation * >( mMapTools->mapTool( QgsAppMapTools::AnnotationEdit ) ), &QgsMapToolModifyAnnotation::selectionCleared,
           mMapStyleWidget, [this] { mMapStyleWidget->setAnnotationItem( nullptr, QString() ); } );

  // request notification of FileOpen events (double clicking a file icon in Mac OS X Finder)
  // should come after fileNewBlank to ensure project is properly set up to receive any data source files
  QgsApplication::setFileOpenEventReceiver( this );

#ifdef ANDROID
  toggleFullScreen();
#endif

  mStartupProfilerWidgetFactory.reset( std::make_unique< QgsProfilerWidgetFactory >( profiler ) );

  auto toggleRevert = [ = ]
  {
    mActionRevertProject->setEnabled( QgsProject::instance()->isDirty() &&!QgsProject::instance()->fileName().isEmpty() );
  };
  connect( QgsProject::instance(), &QgsProject::isDirtyChanged, mActionRevertProject, toggleRevert );
  connect( QgsProject::instance(), &QgsProject::fileNameChanged, mActionRevertProject, toggleRevert );

  connect( QgsProject::instance()->displaySettings(), &QgsProjectDisplaySettings::bearingFormatChanged, this, [ = ]
  {
    mBearingNumericFormat.reset( QgsProject::instance()->displaySettings()->bearingFormat()->clone() );
  } );
  connect( mMapCanvas, &QgsMapCanvas::panDistanceBearingChanged, this, &QgisApp::showPanMessage );

  // SG Change
  // the most important part of the initialization: make sure that people can play puzzle if they need
  //  QgsPuzzleWidget *puzzleWidget = new QgsPuzzleWidget( mMapCanvas );
  //  mCentralContainer->insertWidget( 2, puzzleWidget );
  //  connect( mCoordsEdit, &QgsStatusBarCoordinatesWidget::weAreBored, this, [ this, puzzleWidget ]
  //  {
  //    if ( puzzleWidget->letsGetThePartyStarted() )
  //      mCentralContainer->setCurrentIndex( 2 );
  //  } );
  //  connect( puzzleWidget, &QgsPuzzleWidget::done, this, [ this ]
  //  {
  //    mCentralContainer->setCurrentIndex( 0 );
  //  } );

  mCodeEditorWidgetFactory.reset( std::make_unique< QgsCodeEditorOptionsFactory >() );
  mBabelGpsDevicesWidgetFactory.reset( std::make_unique< QgsGpsDeviceOptionsFactory >() );
  mCustomProjectionsWidgetFactory.reset( std::make_unique< QgsCustomProjectionOptionsFactory >() );

#ifdef HAVE_3D
  m3DOptionsWidgetFactory.reset( std::make_unique< Qgs3DOptionsFactory >() );
#endif
}

QgisApp::QgisApp()
  : QMainWindow( nullptr, Qt::WindowFlags() )
#ifdef Q_OS_MAC
  , mWindowMenu( nullptr )
#endif
{
  sInstance = this;
  //  setupUi( this ); // SG add
  mInternalClipboard = new QgsClipboard;
  mMapCanvas = new QgsMapCanvas();
  connect( mMapCanvas, &QgsMapCanvas::messageEmitted, this, &QgisApp::displayMessage );
  QgsCanvasRefreshBlocker refreshBlocker;
  mLayerTreeView = new QgsLayerTreeView( this );
  QgsLayerTreeModel *model = new QgsLayerTreeModel( QgsProject::instance()->layerTreeRoot(), this );
  mLayerTreeView->setModel( model );
  mUndoWidget = new QgsUndoWidget( nullptr, mMapCanvas );
  mUserInputDockWidget = new QgsUserInputWidget( this );
  mInfoBar = new QgsMessageBar( centralWidget() );
  mLayerTreeView->setMessageBar( mInfoBar );
  mAdvancedDigitizingDockWidget = new QgsAdvancedDigitizingDockWidget( mMapCanvas, this );
  mPanelMenu = new QMenu( this );
  mProgressBar = new QProgressBar( this );
  mStatusBar = new QgsStatusBar( this );
  mMapTools = std::make_unique< QgsAppMapTools >( mMapCanvas, mAdvancedDigitizingDockWidget );
  mDigitizingTechniqueManager = new QgsMapToolsDigitizingTechniqueManager( this );

  mBearingNumericFormat.reset( QgsLocalDefaultSettings::bearingFormat() );
  // More tests may need more members to be initialized
}

QgisApp::~QgisApp()
{
  // shouldn't be needed, but from this stage on, we don't want/need ANY map canvas refreshes to take place
  mFreezeCount = 1000000;

#ifdef HAVE_GEOREFERENCER
  if ( mGeoreferencer )
  {
    delete mGeoreferencer;
    mGeoreferencer = nullptr;
  }
#endif

  mNetworkLoggerWidgetFactory.reset();

  delete mInternalClipboard;
  delete mQgisInterface;
  delete mStyleSheetBuilder;
  delete mDigitizingTechniqueManager;

  if ( QgsMapTool *tool = mMapCanvas->mapTool() )
    mMapCanvas->unsetMapTool( tool );
  mMapTools.reset();

  // must come after deleting map tools
  delete mAdvancedDigitizingDockWidget;
  mAdvancedDigitizingDockWidget = nullptr;

  delete mpMaptip;

  delete mpGpsWidget;

  delete mOverviewMapCursor;

  delete mTracer;

  delete mVectorLayerTools;
  // delete mWelcomePage; // SG Change
  delete mBookMarksDockWidget;

  // Gracefully delete window manager now
  QgsGui::setWindowManager( nullptr );

  deleteLayoutDesigners();
  removeAnnotationItems();

  // cancel request for FileOpen events
  QgsApplication::setFileOpenEventReceiver( nullptr );

  unregisterCustomLayoutDropHandler( mLayoutQptDropHandler );
  unregisterCustomLayoutDropHandler( mLayoutImageDropHandler );

#ifdef WITH_BINDINGS
  delete mPythonUtils;
#endif

  delete mDataSourceManagerDialog;
  qDeleteAll( mCustomDropHandlers );
  qDeleteAll( mCustomLayoutDropHandlers );

  const QList<QgsElevationProfileWidget *> elevationProfileWidgets = findChildren< QgsElevationProfileWidget * >();
  for ( QgsElevationProfileWidget *widget : elevationProfileWidgets )
  {
    widget->cancelJobs();
    delete widget;
  }

  const QList<QgsMapCanvas *> canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
  {
    delete canvas;
  }

  // these may have references to map layers which need to be cleaned up
  mBrowserWidget->close(); // close first, to trigger save of state
  delete mBrowserWidget;
  mBrowserWidget = nullptr;
  delete mBrowserWidget2;
  mBrowserWidget2 = nullptr;
  delete mBrowserModel;
  mBrowserModel = nullptr;
  delete mGeometryValidationDock;
  mGeometryValidationDock = nullptr;
  delete mSnappingUtils;
  mSnappingUtils = nullptr;
  delete mUserInputDockWidget;
  mUserInputDockWidget = nullptr;
  delete mMapStylingDock;
  mMapStylingDock = nullptr;

  QgsGui::nativePlatformInterface()->cleanup();

  // This function *MUST* be the last one called, as it destroys in
  // particular GDAL. As above objects can hold GDAL/OGR objects, it is not
  // safe destroying them afterwards
  //QgsApplication::exitQgis(); // SG change, leaving this causes a crash on exit. Put in destructor of parent
  // Do *NOT* add anything here !
}

void QgisApp::dragEnterEvent( QDragEnterEvent *event )
{
  if ( event->mimeData()->hasUrls() || event->mimeData()->hasFormat( QStringLiteral( "application/x-vnd.qgis.qgis.uri" ) ) )
  {
    // the mime data are coming from layer tree, so ignore that, do not import those layers again
    if ( !event->mimeData()->hasFormat( QStringLiteral( "application/qgis.layertreemodeldata" ) ) )
      event->acceptProposedAction();
  }

  // check if any custom handlers can operate on the data
  const QVector<QPointer<QgsCustomDropHandler >> handlers = mCustomDropHandlers;
  for ( QgsCustomDropHandler *handler : handlers )
  {
    if ( handler && handler->canHandleMimeData( event->mimeData() ) )
    {
      event->acceptProposedAction();
      return;
    }
  }
}

void QgisApp::dropEvent( QDropEvent *event )
{
  // dragging app is locked for the duration of dropEvent. This causes explorer windows to hang
  // while large projects/layers are loaded. So instead we return from dropEvent as quickly as possible
  // and do the actual handling of the drop after a very short timeout
  QTimer *timer = new QTimer( this );
  timer->setSingleShot( true );
  timer->setInterval( 50 );

  // first, allow custom handlers to directly operate on the mime data
  const QVector<QPointer<QgsCustomDropHandler >> handlers = mCustomDropHandlers;
  for ( QgsCustomDropHandler *handler : handlers )
  {
    if ( handler )
    {
      if ( handler->handleMimeDataV2( event->mimeData() ) )
      {
        // custom handler completely handled this data, no further processing required
        event->acceptProposedAction();
        return;
      }

      Q_NOWARN_DEPRECATED_PUSH
      handler->handleMimeData( event->mimeData() );
      Q_NOWARN_DEPRECATED_POP
    }
  }

  // get the file list
  QList<QUrl>::iterator i;
  QList<QUrl>urls = event->mimeData()->urls();
  QStringList files;
  for ( i = urls.begin(); i != urls.end(); ++i )
  {
    QString fileName = i->toLocalFile();
    // seems that some drag and drop operations include an empty url
    // so we test for length to make sure we have something
    if ( !fileName.isEmpty() )
    {
      files << fileName;
    }
  }

  QgsMimeDataUtils::UriList lst;
  if ( QgsMimeDataUtils::isUriList( event->mimeData() ) )
  {
    lst = QgsMimeDataUtils::decodeUriList( event->mimeData() );
  }

  connect( timer, &QTimer::timeout, this, [this, timer, files, lst]
  {
    QgsCanvasRefreshBlocker refreshBlocker;

    for ( const QString &file : std::as_const( files ) )
    {
      bool handled = false;

      // give custom drop handlers first priority at handling the file
      const QVector<QPointer<QgsCustomDropHandler >> handlers = mCustomDropHandlers;
      for ( QgsCustomDropHandler *handler : handlers )
      {
        if ( handler && handler->handleFileDrop( file ) )
        {
          handled = true;
          break;
        }
      }

      if ( !handled )
        openFile( file );
    }

    if ( !lst.isEmpty() )
    {
      handleDropUriList( lst );
    }

    timer->deleteLater();
  } );

  event->acceptProposedAction();
  timer->start();
}

void QgisApp::annotationCreated( QgsAnnotation *annotation )
{
  const auto canvases = mapCanvases();
  // create canvas annotation item for annotation
  for ( QgsMapCanvas *canvas : canvases )
  {
    QgsMapCanvasAnnotationItem *canvasItem = new QgsMapCanvasAnnotationItem( annotation, canvas );
    Q_UNUSED( canvasItem ) //item is already added automatically to canvas scene
  }
}

void QgisApp::registerCustomDropHandler( QgsCustomDropHandler *handler )
{
  if ( !mCustomDropHandlers.contains( handler ) )
    mCustomDropHandlers << handler;

  const auto canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
  {
    canvas->setCustomDropHandlers( mCustomDropHandlers );
  }
}

void QgisApp::unregisterCustomDropHandler( QgsCustomDropHandler *handler )
{
  mCustomDropHandlers.removeOne( handler );

  const auto canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
  {
    canvas->setCustomDropHandlers( mCustomDropHandlers );
  }
}

void QgisApp::registerCustomProjectOpenHandler( QgsCustomProjectOpenHandler *handler )
{
  if ( !mCustomProjectOpenHandlers.contains( handler ) )
    mCustomProjectOpenHandlers << handler;
}

void QgisApp::unregisterCustomProjectOpenHandler( QgsCustomProjectOpenHandler *handler )
{
  mCustomProjectOpenHandlers.removeOne( handler );
}

QVector<QPointer<QgsCustomDropHandler> > QgisApp::customDropHandlers() const
{
  return mCustomDropHandlers;
}

void QgisApp::registerCustomLayoutDropHandler( QgsLayoutCustomDropHandler *handler )
{
  if ( !mCustomLayoutDropHandlers.contains( handler ) )
    mCustomLayoutDropHandlers << handler;
}

void QgisApp::unregisterCustomLayoutDropHandler( QgsLayoutCustomDropHandler *handler )
{
  mCustomLayoutDropHandlers.removeOne( handler );
}

QVector<QPointer<QgsLayoutCustomDropHandler> > QgisApp::customLayoutDropHandlers() const
{
  return mCustomLayoutDropHandlers;
}

void QgisApp::handleDropUriList( const QgsMimeDataUtils::UriList &lst )
{
  // avoid unnecessary work when adding lots of layers at once - defer emitting the active layer changed signal until we've
  // added all layers, and only emit the signal once for the final layer added
  mBlockActiveLayerChanged = true;

  QgsScopedProxyProgressTask task( tr( "Loading layers" ) );


  auto showLayerLoadWarnings = [ = ]( const QString & title, const QString & shortMessage, const QString & longMessage, Qgis::MessageLevel level )
  {
    QgsMessageBarItem *messageWidget = QgsMessageBar::createMessage( title, shortMessage );
    QPushButton *detailsButton = new QPushButton( tr( "Details" ) );
    connect( detailsButton, &QPushButton::clicked, this, [ = ]
    {
      if ( QgsMessageViewer *dialog = dynamic_cast< QgsMessageViewer * >( QgsMessageOutput::createMessageOutput() ) )
      {
        dialog->setTitle( title );
        dialog->setMessage( longMessage, QgsMessageOutput::MessageHtml );
        dialog->showMessage();
      }
    } );
    messageWidget->layout()->addWidget( detailsButton );
    return visibleMessageBar()->pushWidget( messageWidget, level, 0 );
  };

  // insert items in reverse order as each one is inserted on top of previous one
  int count = 0;
  for ( int i = lst.size() - 1 ; i >= 0 ; i--, count++ )
  {
    const QgsMimeDataUtils::Uri &u = lst.at( i );

    QString uri = crsAndFormatAdjustedLayerUri( u.uri, u.supportedCrs, u.supportedFormats );

    if ( u.layerType == QLatin1String( "collection" ) )
    {
      openLayer( uri, true );
    }
    else if ( u.layerType == QLatin1String( "vector" ) )
    {
      addVectorLayer( uri, u.name, u.providerKey );
    }
    else if ( u.layerType == QLatin1String( "raster" ) )
    {
      addRasterLayer( uri, u.name, u.providerKey );
    }
    else if ( u.layerType == QLatin1String( "mesh" ) )
    {
      addMeshLayer( uri, u.name, u.providerKey );
    }
    else if ( u.layerType == QLatin1String( "pointcloud" ) )
    {
      addPointCloudLayer( uri, u.name, u.providerKey );
    }
    else if ( u.layerType == QLatin1String( "vector-tile" ) )
    {
      QgsTemporaryCursorOverride busyCursor( Qt::WaitCursor );

      const QgsVectorTileLayer::LayerOptions options( QgsProject::instance()->transformContext() );
      QgsVectorTileLayer *layer = new QgsVectorTileLayer( uri, u.name, options );
      bool ok = false;
      layer->loadDefaultMetadata( ok );

      QString error;
      QStringList warnings;
      bool res = layer->loadDefaultStyle( error, warnings );
      if ( res && !warnings.empty() )
      {
        QString message = QStringLiteral( "<p>%1</p>" ).arg( tr( "The following warnings were generated while converting the vector tile style:" ) );
        message += QLatin1String( "<ul>" );

        std::sort( warnings.begin(), warnings.end() );
        warnings.erase( std::unique( warnings.begin(), warnings.end() ), warnings.end() );

        for ( const QString &w : std::as_const( warnings ) )
        {
          message += QStringLiteral( "<li>%1</li>" ).arg( w.toHtmlEscaped().replace( '\n', QLatin1String( "<br>" ) ) );
        }
        message += QLatin1String( "</ul>" );
        showLayerLoadWarnings( tr( "Vector tiles" ), tr( "Style could not be completely converted" ),
                               message, Qgis::MessageLevel::Warning );
      }
      addMapLayer( layer );
    }
    else if ( u.layerType == QLatin1String( "plugin" ) )
    {
      addPluginLayer( uri, u.name, u.providerKey );
    }
    else if ( u.layerType == QLatin1String( "custom" ) )
    {
      const auto constMCustomDropHandlers = mCustomDropHandlers;
      for ( QgsCustomDropHandler *handler : constMCustomDropHandlers )
      {
        if ( handler && handler->customUriProviderKey() == u.providerKey )
        {
          handler->handleCustomUriDrop( u );
          break;
        }
      }
    }
    else if ( u.layerType == QLatin1String( "project" ) )
    {
      openFile( u.uri, QStringLiteral( "project" ) );
    }

    task.setProgress( 100.0 * static_cast< double >( count ) / lst.size() );
  }

  mBlockActiveLayerChanged = false;
  onActiveLayerChanged( activeLayer() );
}

bool QgisApp::event( QEvent *event )
{
  bool done = false;
  if ( event->type() == QEvent::FileOpen )
  {
    // handle FileOpen event (double clicking a file icon in Mac OS X Finder)
    QFileOpenEvent *foe = static_cast<QFileOpenEvent *>( event );
    openFile( foe->file() );
    done = true;
  }
  else if ( !QTouchDevice::devices().isEmpty() && event->type() == QEvent::Gesture )
  {
    done = gestureEvent( static_cast<QGestureEvent *>( event ) );
  }
  else
  {
    // pass other events to base class
    done = QMainWindow::event( event );
  }
  return done;
}


QgsMessageBar *QgisApp::visibleMessageBar()
{
  if ( mDataSourceManagerDialog &&
       mDataSourceManagerDialog->isVisible() &&
       mDataSourceManagerDialog->isModal() )
  {
    return mDataSourceManagerDialog->messageBar();
  }
  else
  {
    return messageBar();
  }
}

const QList<QgsVectorLayerRef> QgisApp::findBrokenLayerDependencies( QgsVectorLayer *vl, QgsMapLayer::StyleCategories categories ) const
{
  QList<QgsVectorLayerRef> brokenDependencies;

  if ( categories.testFlag( QgsMapLayer::StyleCategory::Forms ) )
  {
    for ( int i = 0; i < vl->fields().count(); i++ )
    {
      const QgsEditorWidgetSetup setup = QgsGui::editorWidgetRegistry()->findBest( vl, vl->fields().field( i ).name() );
      QgsFieldFormatter *fieldFormatter = QgsApplication::fieldFormatterRegistry()->fieldFormatter( setup.type() );
      if ( fieldFormatter )
      {
        const QList<QgsVectorLayerRef> constDependencies { fieldFormatter->layerDependencies( setup.config() ) };
        for ( const QgsVectorLayerRef &dependency : constDependencies )
        {
          // I guess we need and isNull()/isValid() method for the ref
          if ( dependency.layer ||
               ! dependency.name.isEmpty() ||
               ! dependency.source.isEmpty() ||
               ! dependency.layerId.isEmpty() )
          {
            const QgsVectorLayer *depVl { QgsVectorLayerRef( dependency ).resolveWeakly( QgsProject::instance(), QgsVectorLayerRef::MatchType::Name ) };
            if ( ! depVl || ! depVl->isValid() )
            {
              brokenDependencies.append( dependency );
            }
          }
        }
      }
    }
  }

  if ( categories.testFlag( QgsMapLayer::StyleCategory::Relations ) )
  {
    // Check for layer weak relations
    const QList<QgsWeakRelation> constWeakRelations { vl->weakRelations() };
    for ( const QgsWeakRelation &rel : constWeakRelations )
    {
      QgsRelation relation { rel.resolvedRelation( QgsProject::instance(), QgsVectorLayerRef::MatchType::Name ) };
      QgsVectorLayerRef dependency;
      bool found = false;
      if ( ! relation.isValid() )
      {
        // This is the big question: do we really
        // want to automatically load the referencing layer(s) too?
        // This could potentially lead to a cascaded load of a
        // long list of layers.
        // The code is in place but let's leave it disabled for now.
        if ( relation.referencedLayer() == vl )
        {
          // Do nothing because vl is the referenced layer
#if 0
          dependency = rel.referencingLayer();
          found = true;
#endif
        }
        else if ( relation.referencingLayer() == vl )
        {
          dependency = rel.referencedLayer();
          found = true;
        }
        else
        {
          // Something wrong is going on here, maybe this relation
          // does not really apply to this layer?
          QgsMessageLog::logMessage( tr( "None of the layers in the relation stored in the style match the current layer, skipping relation id: %1." ).arg( relation.id() ) );
        }

        if ( found )
        {
          // Make sure we don't add it twice if it was already added by the form widgets check
          bool refFound = false;
          for ( const QgsVectorLayerRef &otherRef : std::as_const( brokenDependencies ) )
          {
            if ( dependency.layerId == otherRef.layerId || ( dependency.source == otherRef.source && dependency.provider == otherRef.provider ) )
            {
              refFound = true;
              break;
            }
          }
          if ( ! refFound )
          {
            brokenDependencies.append( dependency );
          }
        }
      }
    }
  }
  return brokenDependencies;
}

void QgisApp::resolveVectorLayerDependencies( QgsVectorLayer *vl, QgsMapLayer::StyleCategories categories )
{
  if ( vl && vl->isValid() )
  {
    const auto constDependencies { findBrokenLayerDependencies( vl, categories ) };
    for ( const QgsVectorLayerRef &dependency : constDependencies )
    {
      // Check for projects without layer dependencies (see 7e8c7b3d0e094737336ff4834ea2af625d2921bf)
      if ( QgsProject::instance()->mapLayer( dependency.layerId ) || ( dependency.name.isEmpty() && dependency.source.isEmpty() ) )
      {
        continue;
      }
      // try to aggressively resolve the broken dependencies
      bool loaded = false;
      const QString providerName { vl->dataProvider()->name() };
      QgsProviderMetadata *providerMetadata { QgsProviderRegistry::instance()->providerMetadata( providerName ) };
      if ( providerMetadata )
      {
        // Retrieve the DB connection (if any)

        std::unique_ptr< QgsAbstractDatabaseProviderConnection > conn { QgsMapLayerUtils::databaseConnection( vl ) };
        if ( conn )
        {
          QString tableSchema;
          QString tableName;
          const QVariantMap sourceParts = providerMetadata->decodeUri( dependency.source );

          // This part should really be abstracted out to the connection classes or to the providers directly.
          // Different providers decode the uri differently, for example we don't get the table name out of OGR
          // but the layerName/layerId instead, so let's try different approaches

          // This works for GPKG
          tableName = sourceParts.value( QStringLiteral( "layerName" ) ).toString();

          // This works for PG and spatialite
          if ( tableName.isEmpty() )
          {
            tableName = sourceParts.value( QStringLiteral( "table" ) ).toString();
            tableSchema = sourceParts.value( QStringLiteral( "schema" ) ).toString();
          }

          // Helper to find layers in connections
          auto layerFinder = [ &conn, &dependency, &providerName ]( const QString & tableSchema, const QString & tableName ) -> bool
          {
            // First try the current schema (or no schema if it's not supported from the provider)
            try
            {
              const QString layerUri { conn->tableUri( tableSchema, tableName )};
              // Load it!
              std::unique_ptr< QgsVectorLayer > newVl = std::make_unique< QgsVectorLayer >( layerUri, dependency.name, providerName );
              if ( newVl->isValid() )
              {
                QgsProject::instance()->addMapLayer( newVl.release() );
                return true;
              }
            }
            catch ( QgsProviderConnectionException & )
            {
              // Do nothing!
            }
            return false;
          };

          loaded = layerFinder( tableSchema, tableName );

          // Try different schemas
          if ( ! loaded && conn->capabilities().testFlag( QgsAbstractDatabaseProviderConnection::Capability::Schemas ) && ! tableSchema.isEmpty() )
          {
            const QStringList schemas { conn->schemas() };
            for ( const QString &schemaName : schemas )
            {
              if ( schemaName != tableSchema )
              {
                loaded = layerFinder( schemaName, tableName );
              }
              if ( loaded )
              {
                break;
              }
            }
          }
        }
      }
      if ( ! loaded )
      {
        const QString msg { tr( "layer '%1' requires layer '%2' to be loaded but '%2' could not be found, please load it manually if possible." ).arg( vl->name(), dependency.name ) };
        messageBar()->pushWarning( tr( "Missing layer form dependency" ), msg );
      }
      else
      {
        messageBar()->pushSuccess( tr( "Missing layer form dependency" ), tr( "Layer dependency '%2' required by '%1' was automatically loaded." )
                                   .arg( vl->name() )
                                   .arg( dependency.name ) );
      }
    }
  }
}

void QgisApp::resolveVectorLayerWeakRelations( QgsVectorLayer *vectorLayer )
{
  if ( vectorLayer && vectorLayer->isValid() )
  {
    const QList<QgsWeakRelation> constWeakRelations { vectorLayer->weakRelations( ) };
    for ( const QgsWeakRelation &rel : constWeakRelations )
    {
      QgsRelation relation { rel.resolvedRelation( QgsProject::instance(), QgsVectorLayerRef::MatchType::Name ) };
      if ( relation.isValid() )
      {
        // Avoid duplicates
        const QList<QgsRelation> constRelations { QgsProject::instance()->relationManager()->relations().values() };
        for ( const QgsRelation &other : constRelations )
        {
          if ( relation.hasEqualDefinition( other ) )
          {
            continue;
          }
        }
        QgsProject::instance()->relationManager()->addRelation( relation );
      }
    }
  }
}

void QgisApp::dataSourceManager( const QString &pageName )
{
  if ( ! mDataSourceManagerDialog )
  {
    mDataSourceManagerDialog = new QgsDataSourceManagerDialog( mBrowserModel, this, mapCanvas() );
    connect( this, &QgisApp::connectionsChanged, mDataSourceManagerDialog, &QgsDataSourceManagerDialog::refresh );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::connectionsChanged, this, &QgisApp::connectionsChanged );
    connect( mDataSourceManagerDialog, SIGNAL( addRasterLayer( QString const &, QString const &, QString const & ) ),
             this, SLOT( addRasterLayer( QString const &, QString const &, QString const & ) ) );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::addRasterLayers, this, [ = ]( const QStringList & layersList ) { addRasterLayers( layersList ); } );
    connect( mDataSourceManagerDialog, SIGNAL( addVectorLayer( QString const &, QString const &, QString const & ) ),
             this, SLOT( addVectorLayer( QString const &, QString const &, QString const & ) ) );
    connect( mDataSourceManagerDialog, SIGNAL( addVectorLayers( QStringList const &, QString const &, QString const & ) ),
             this, SLOT( addVectorLayers( QStringList const &, QString const &, QString const & ) ) );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::addMeshLayer, this, &QgisApp::addMeshLayer );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::addVectorTileLayer, this, &QgisApp::addVectorTileLayer );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::addPointCloudLayer, this, &QgisApp::addPointCloudLayer );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::showStatusMessage, this, &QgisApp::showStatusMessage );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::addDatabaseLayers, this, &QgisApp::addDatabaseLayers );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::replaceSelectedVectorLayer, this, &QgisApp::replaceSelectedVectorLayer );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::handleDropUriList, this, &QgisApp::handleDropUriList );
    connect( this, &QgisApp::newProject, mDataSourceManagerDialog, &QgsDataSourceManagerDialog::updateProjectHome );
    connect( mDataSourceManagerDialog, &QgsDataSourceManagerDialog::openFile, this, &QgisApp::openFile );

  }
  else
  {
    mDataSourceManagerDialog->reset();
  }
  // Try to open the dialog on a particular page
  if ( ! pageName.isEmpty() )
  {
    mDataSourceManagerDialog->openPage( pageName );
  }
  if ( QgsSettings().value( QStringLiteral( "/qgis/dataSourceManagerNonModal" ), true ).toBool() )
  {
    mDataSourceManagerDialog->show();
  }
  else
  {
    mDataSourceManagerDialog->exec();
  }
}

QgsBrowserGuiModel *QgisApp::browserModel()
{
  return mBrowserModel;
}

QgisAppStyleSheet *QgisApp::styleSheetBuilder()
{
  Q_ASSERT( mStyleSheetBuilder );
  return mStyleSheetBuilder;
}

void QgisApp::readRecentProjects()
{
  QgsSettings settings;
  mRecentProjects.clear();

  settings.beginGroup( QStringLiteral( "UI" ) );

  // Migrate old recent projects if first time with new system
  if ( !settings.childGroups().contains( QStringLiteral( "recentProjects" ) ) )
  {
    QStringList oldRecentProjects = settings.value( QStringLiteral( "UI/recentProjectsList" ) ).toStringList();

    const auto constOldRecentProjects = oldRecentProjects;
    for ( const QString &project : constOldRecentProjects )
    {
      QgsRecentProjectItemsModel::RecentProjectData data;
      data.path = project;
      data.title = project;

      mRecentProjects.append( data );
    }
  }
  settings.endGroup();

  settings.beginGroup( QStringLiteral( "UI/recentProjects" ) );
  QStringList projectKeysList = settings.childGroups();

  //convert list to int values to obtain proper order
  QList<int> projectKeys;
  const auto constProjectKeysList = projectKeysList;
  for ( const QString &key : constProjectKeysList )
  {
    projectKeys.append( key.toInt() );
  }
  std::sort( projectKeys.begin(), projectKeys.end() );

  int pinPos = 0;
  const int maxProjects = QgsSettings().value( QStringLiteral( "maxRecentProjects" ), 20, QgsSettings::App ).toInt();
  for ( int i = 0; i < projectKeys.count(); ++i )
  {
    QgsRecentProjectItemsModel::RecentProjectData data;
    settings.beginGroup( QString::number( projectKeys.at( i ) ) );
    data.title = settings.value( QStringLiteral( "title" ) ).toString();
    data.path = settings.value( QStringLiteral( "path" ) ).toString();
    data.previewImagePath = settings.value( QStringLiteral( "previewImage" ) ).toString();
    data.crs = settings.value( QStringLiteral( "crs" ) ).toString();
    data.pin = settings.value( QStringLiteral( "pin" ) ).toBool();
    settings.endGroup();
    if ( data.pin )
    {
      mRecentProjects.insert( pinPos, data );
      pinPos++;
    }
    else
    {
      mRecentProjects.append( data );
    }
    if ( mRecentProjects.count() >= maxProjects )
      break;
  }
  settings.endGroup();
}

void QgisApp::applyProjectSettingsToCanvas( QgsMapCanvas *canvas )
{
  canvas->setCanvasColor( QgsProject::instance()->backgroundColor() );
  canvas->setSelectionColor( QgsProject::instance()->selectionColor() );
}

void QgisApp::applyDefaultSettingsToCanvas( QgsMapCanvas *canvas )
{
  QgsSettings settings;
  canvas->enableAntiAliasing( settings.value( QStringLiteral( "qgis/enable_anti_aliasing" ), true ).toBool() );
  double zoomFactor = settings.value( QStringLiteral( "qgis/zoom_factor" ), 2 ).toDouble();
  canvas->setWheelFactor( zoomFactor );
  canvas->setCachingEnabled( settings.value( QStringLiteral( "qgis/enable_render_caching" ), true ).toBool() );
  canvas->setParallelRenderingEnabled( settings.value( QStringLiteral( "qgis/parallel_rendering" ), true ).toBool() );
  canvas->setMapUpdateInterval( settings.value( QStringLiteral( "qgis/map_update_interval" ), 250 ).toInt() );
  canvas->setSegmentationTolerance( settings.value( QStringLiteral( "qgis/segmentationTolerance" ), "0.01745" ).toDouble() );
  canvas->setSegmentationToleranceType( QgsAbstractGeometry::SegmentationToleranceType( settings.enumValue( QStringLiteral( "qgis/segmentationToleranceType" ), QgsAbstractGeometry::MaximumAngle ) ) );
}

int QgisApp::chooseReasonableDefaultIconSize() const
{
  QScreen *screen = QApplication::screens().at( 0 );
  if ( screen->physicalDotsPerInch() < 115 )
  {
    // no hidpi screen, use default size
    return QGIS_ICON_SIZE;
  }
  else
  {
    double size = fontMetrics().horizontalAdvance( 'X' ) * 3;
    if ( size < 24 )
      return 16;
    else if ( size < 32 )
      return 24;
    else if ( size < 48 )
      return 32;
    else if ( size < 64 )
      return 48;
    else
      return 64;
  }

}

void QgisApp::readSettings()
{
  QgsSettings settings;
  QString themeName = settings.value( QStringLiteral( "UI/UITheme" ), "default" ).toString();
  setTheme( themeName );

  // Read legacy settings
  readRecentProjects();

  // this is a new session, reset enable macros value  when they are set for session
  Qgis::PythonMacroMode macroMode = settings.enumValue( QStringLiteral( "qgis/enableMacros" ), Qgis::PythonMacroMode::Ask );
  switch ( macroMode )
  {
    case Qgis::PythonMacroMode::NotForThisSession:
    case Qgis::PythonMacroMode::SessionOnly:
      settings.setEnumValue( QStringLiteral( "qgis/enableMacros" ), Qgis::PythonMacroMode::Ask );
      break;

    case Qgis::PythonMacroMode::Always:
    case Qgis::PythonMacroMode::Never:
    case Qgis::PythonMacroMode::Ask:
      break;
  }
}


//////////////////////////////////////////////////////////////////////
//            Set Up the gui toolbars, menus, statusbar etc
//////////////////////////////////////////////////////////////////////

void QgisApp::createActions()
{
  mActionPluginSeparator1 = nullptr;  // plugin list separator will be created when the first plugin is loaded
  mActionPluginSeparator2 = nullptr;  // python separator will be created only if python is found
  mActionRasterSeparator = nullptr;   // raster plugins list separator will be created when the first plugin is loaded

  // Project Menu Items

  connect( mActionNewProject, &QAction::triggered, this, [ = ] { fileNew(); } );
  connect( mActionNewBlankProject, &QAction::triggered, this, &QgisApp::fileNewBlank );
  connect( mActionOpenProject, &QAction::triggered, this, &QgisApp::fileOpen );
  connect( mActionRevertProject, &QAction::triggered, this, &QgisApp::fileRevert );
  connect( mActionSaveProject, &QAction::triggered, this, &QgisApp::fileSave );
  connect( mActionCloseProject, &QAction::triggered, this, &QgisApp::fileClose );
  connect( mActionSaveProjectAs, &QAction::triggered, this, &QgisApp::fileSaveAs );
  connect( mActionSaveMapAsImage, &QAction::triggered, this, [ = ] { saveMapAsImage(); } );
  connect( mActionSaveMapAsPdf, &QAction::triggered, this, [ = ] { saveMapAsPdf(); } );
  connect( mActionNewMapCanvas, &QAction::triggered, this, &QgisApp::newMapCanvas );
  connect( mActionNew3DMapCanvas, &QAction::triggered, this, &QgisApp::new3DMapCanvas );
  connect( mActionNewPrintLayout, &QAction::triggered, this, &QgisApp::newPrintLayout );
  connect( mActionNewReport, &QAction::triggered, this, &QgisApp::newReport );
  connect( mActionShowLayoutManager, &QAction::triggered, this, &QgisApp::showLayoutManager );
  connect( mActionExit, &QAction::triggered, this, &QgisApp::fileExit );
  connect( mActionDxfExport, &QAction::triggered, this, &QgisApp::dxfExport );
  connect( mActionDwgImport, &QAction::triggered, this, &QgisApp::dwgImport );

  // Edit Menu Items

  connect( mActionUndo, &QAction::triggered, mUndoWidget, &QgsUndoWidget::undo );
  connect( mActionRedo, &QAction::triggered, mUndoWidget, &QgsUndoWidget::redo );
  connect( mActionCutFeatures, &QAction::triggered, this, [ = ] { cutSelectionToClipboard(); } );
  connect( mActionCopyFeatures, &QAction::triggered, this, [ = ] { copySelectionToClipboard(); } );
  connect( mActionPasteFeatures, &QAction::triggered, this, [ = ] { pasteFromClipboard(); } );
  connect( mActionPasteAsNewVector, &QAction::triggered, this, &QgisApp::pasteAsNewVector );
  connect( mActionPasteAsNewMemoryVector, &QAction::triggered, this, [ = ] { pasteAsNewMemoryVector(); } );
  connect( mActionCopyStyle, &QAction::triggered, this, [ = ] { copyStyle(); } );
  connect( mActionPasteStyle, &QAction::triggered, this, [ = ] { pasteStyle(); } );
  connect( mActionCopyLayer, &QAction::triggered, this, &QgisApp::copyLayer );
  connect( mActionPasteLayer, &QAction::triggered, this, &QgisApp::pasteLayer );
  connect( mActionAddFeature, &QAction::triggered, this, &QgisApp::addFeature );

  connect( mActionMoveFeature, &QAction::triggered, this, &QgisApp::moveFeature );
  connect( mActionMoveFeatureCopy, &QAction::triggered, this, &QgisApp::moveFeatureCopy );
  connect( mActionRotateFeature, &QAction::triggered, this, &QgisApp::rotateFeature );
  connect( mActionScaleFeature, &QAction::triggered, this, &QgisApp::scaleFeature );
  connect( mActionReshapeFeatures, &QAction::triggered, this, &QgisApp::reshapeFeatures );
  connect( mActionSplitFeatures, &QAction::triggered, this, &QgisApp::splitFeatures );
  connect( mActionSplitParts, &QAction::triggered, this, &QgisApp::splitParts );
  connect( mActionDeleteSelected, &QAction::triggered, this, [ = ] { deleteSelected( nullptr, nullptr, true ); } );
  connect( mActionAddRing, &QAction::triggered, this, &QgisApp::addRing );
  connect( mActionFillRing, &QAction::triggered, this, &QgisApp::fillRing );
  connect( mActionAddPart, &QAction::triggered, this, &QgisApp::addPart );
  connect( mActionSimplifyFeature, &QAction::triggered, this, &QgisApp::simplifyFeature );
  connect( mActionDeleteRing, &QAction::triggered, this, &QgisApp::deleteRing );
  connect( mActionDeletePart, &QAction::triggered, this, &QgisApp::deletePart );
  connect( mActionMergeFeatures, &QAction::triggered, this, &QgisApp::mergeSelectedFeatures );
  connect( mActionMergeFeatureAttributes, &QAction::triggered, this, &QgisApp::mergeAttributesOfSelectedFeatures );
  connect( mActionMultiEditAttributes, &QAction::triggered, this, &QgisApp::modifyAttributesOfSelectedFeatures );
  connect( mActionVertexTool, &QAction::triggered, this, &QgisApp::vertexTool );
  connect( mActionVertexToolActiveLayer, &QAction::triggered, this, &QgisApp::vertexToolActiveLayer );
  connect( mActionRotatePointSymbols, &QAction::triggered, this, &QgisApp::rotatePointSymbols );
  connect( mActionOffsetPointSymbol, &QAction::triggered, this, &QgisApp::offsetPointSymbol );
  connect( mActionSnappingOptions, &QAction::triggered, this, &QgisApp::snappingOptions );
  connect( mActionOffsetCurve, &QAction::triggered, this, &QgisApp::offsetCurve );
  connect( mActionReverseLine, &QAction::triggered, this, &QgisApp::reverseLine );
  connect( mActionTrimExtendFeature, &QAction::triggered, this, [ = ] { mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::TrimExtendFeature ) ); } );

  // View Menu Items
  connect( mActionPan, &QAction::triggered, this, &QgisApp::pan );
  connect( mActionPanToSelected, &QAction::triggered, this, &QgisApp::panToSelected );
  connect( mActionZoomIn, &QAction::triggered, this, &QgisApp::zoomIn );
  connect( mActionZoomOut, &QAction::triggered, this, &QgisApp::zoomOut );
  connect( mActionSelectFeatures, &QAction::triggered, this, &QgisApp::selectFeatures );
  connect( mActionSelectPolygon, &QAction::triggered, this, &QgisApp::selectByPolygon );
  connect( mActionSelectFreehand, &QAction::triggered, this, &QgisApp::selectByFreehand );
  connect( mActionSelectRadius, &QAction::triggered, this, &QgisApp::selectByRadius );
  connect( mActionDeselectAll, &QAction::triggered, this, &QgisApp::deselectAll );
  connect( mActionDeselectActiveLayer, &QAction::triggered, this, &QgisApp::deselectActiveLayer );
  connect( mActionSelectAll, &QAction::triggered, this, &QgisApp::selectAll );
  connect( mActionReselect, &QAction::triggered, this, [ = ]
  {
    QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mMapCanvas->currentLayer() );
    if ( !vlayer )
    {
      visibleMessageBar()->pushMessage(
        tr( "No active vector layer" ),
        tr( "To reselect features, choose a vector layer in the legend." ),
        Qgis::MessageLevel::Info );
      return;
    }

    vlayer->reselect();
  } );
  connect( mActionInvertSelection, &QAction::triggered, this, &QgisApp::invertSelection );
  connect( mActionSelectByExpression, &QAction::triggered, this, &QgisApp::selectByExpression );
  connect( mActionSelectByForm, &QAction::triggered, this, &QgisApp::selectByForm );
  connect( mActionIdentify, &QAction::triggered, this, &QgisApp::identify );
  connect( mActionFeatureAction, &QAction::triggered, this, &QgisApp::doFeatureAction );
  connect( mActionMeasure, &QAction::triggered, this, &QgisApp::measure );
  connect( mActionMeasureArea, &QAction::triggered, this, &QgisApp::measureArea );
  connect( mActionMeasureAngle, &QAction::triggered, this, &QgisApp::measureAngle );
  connect( mActionMeasureBearing, &QAction::triggered, this,  [ = ] { setMapTool( mMapTools->mapTool( QgsAppMapTools::MeasureBearing ) ); } );
  connect( mActionZoomFullExtent, &QAction::triggered, this, &QgisApp::zoomFull );
  connect( mActionZoomToLayer, &QAction::triggered, this, &QgisApp::zoomToLayerExtent );
  connect( mActionZoomToLayers, &QAction::triggered, this, &QgisApp::zoomToLayerExtent );
  connect( mActionZoomToSelected, &QAction::triggered, this, &QgisApp::zoomToSelected );
  connect( mActionZoomLast, &QAction::triggered, this, &QgisApp::zoomToPrevious );
  connect( mActionZoomNext, &QAction::triggered, this, &QgisApp::zoomToNext );
  connect( mActionZoomActualSize, &QAction::triggered, this, &QgisApp::zoomActualSize );
  connect( mActionMapTips, &QAction::toggled, this, &QgisApp::toggleMapTips );
  connect( mActionNewBookmark, &QAction::triggered, this, [ = ] { newBookmark( true );  } );
  connect( mActionDraw, &QAction::triggered, this, [this] { refreshMapCanvas( true ); } );
  connect( mActionTextAnnotation, &QAction::triggered, this, &QgisApp::addTextAnnotation );
  connect( mActionFormAnnotation, &QAction::triggered, this, &QgisApp::addFormAnnotation );
  connect( mActionHtmlAnnotation, &QAction::triggered, this, &QgisApp::addHtmlAnnotation );
  connect( mActionSvgAnnotation, &QAction::triggered, this, &QgisApp::addSvgAnnotation );
  connect( mActionAnnotation, &QAction::triggered, this, &QgisApp::modifyAnnotation );
  connect( mActionLabeling, &QAction::triggered, this, &QgisApp::labeling );
  mStatisticalSummaryDockWidget->setToggleVisibilityAction( mActionStatisticalSummary );
  connect( mActionManage3DMapViews, &QAction::triggered, this, &QgisApp::show3DMapViewsManager );
  connect( mActionElevationProfile, &QAction::triggered, this, &QgisApp::createNewElevationProfile );

  // Layer Menu Items

  connect( mActionDataSourceManager, &QAction::triggered, this, [ = ]() { dataSourceManager(); } );
  connect( mActionNewVectorLayer, &QAction::triggered, this, &QgisApp::newVectorLayer );
#ifdef HAVE_SPATIALITE
  connect( mActionNewSpatiaLiteLayer, &QAction::triggered, this, &QgisApp::newSpatialiteLayer );
#endif
  connect( mActionNewGeoPackageLayer, &QAction::triggered, this, &QgisApp::newGeoPackageLayer );
  connect( mActionNewMemoryLayer, &QAction::triggered, this, &QgisApp::newMemoryLayer );
  connect( mActionNewMeshLayer, &QAction::triggered, this, &QgisApp::newMeshLayer );
  connect( mActionNewGpxLayer, &QAction::triggered, this, &QgisApp::newGpxLayer );
  connect( mActionNewVirtualLayer, &QAction::triggered, this, &QgisApp::addVirtualLayer );
  connect( mActionShowRasterCalculator, &QAction::triggered, this, &QgisApp::showRasterCalculator );
  connect( mActionShowMeshCalculator, &QAction::triggered, this, &QgisApp::showMeshCalculator );
  connect( mActionShowAlignRasterTool, &QAction::triggered, this, &QgisApp::showAlignRasterTool );
  connect( mActionEmbedLayers, &QAction::triggered, this, &QgisApp::embedLayers );
  connect( mActionAddLayerDefinition, &QAction::triggered, this, &QgisApp::addLayerDefinition );
  connect( mActionAddOgrLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "ogr" ) ); } );
  connect( mActionAddRasterLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "gdal" ) ); } );
  connect( mActionAddMeshLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "mdal" ) ); } );
  connect( mActionAddPgLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "postgres" ) ); } );
#ifdef HAVE_SPATIALITE
  connect( mActionAddSpatiaLiteLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "spatialite" ) ); } );
#endif
  connect( mActionAddMssqlLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "mssql" ) ); } );
  connect( mActionAddOracleLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "oracle" ) ); } );
  connect( mActionAddHanaLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "hana" ) ); } );
  connect( mActionAddWmsLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "wms" ) ); } );
  connect( mActionAddXyzLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "xyz" ) ); } );
  connect( mActionAddVectorTileLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "vectortile" ) ); } );
  connect( mActionAddPointCloudLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "pointcloud" ) ); } );
  connect( mActionAddWcsLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "wcs" ) ); } );
#ifdef HAVE_SPATIALITE
  connect( mActionAddWfsLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "WFS" ) ); } );
#endif
  connect( mActionAddAfsLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "arcgisfeatureserver" ) ); } );
  connect( mActionAddDelimitedText, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "delimitedtext" ) ); } );
  connect( mActionAddVirtualLayer, &QAction::triggered, this, [ = ] { dataSourceManager( QStringLiteral( "virtual" ) ); } );
  connect( mActionOpenTable, &QAction::triggered, this, [ = ]
  {
    QgsSettings settings;
    QgsAttributeTableFilterModel::FilterMode initialMode = settings.enumValue( QStringLiteral( "qgis/attributeTableBehavior" ),  QgsAttributeTableFilterModel::ShowAll );
    attributeTable( initialMode );
  } );
  connect( mActionOpenTableSelected, &QAction::triggered, this, [ = ]
  {
    attributeTable( QgsAttributeTableFilterModel::ShowSelected );
  } );
  connect( mActionOpenTableVisible, &QAction::triggered, this, [ = ]
  {
    attributeTable( QgsAttributeTableFilterModel::ShowVisible );
  } );
  connect( mActionOpenTableEdited, &QAction::triggered, this, [ = ]
  {
    attributeTable( QgsAttributeTableFilterModel::ShowEdited );
  } );
  connect( mActionOpenFieldCalc, &QAction::triggered, this, &QgisApp::fieldCalculator );
  connect( mActionToggleEditing, &QAction::triggered, this, [ = ] { toggleEditing(); } );
  connect( mActionSaveLayerEdits, &QAction::triggered, this, &QgisApp::saveActiveLayerEdits );
  connect( mActionSaveEdits, &QAction::triggered, this, [ = ] { saveEdits(); } );
  connect( mActionSaveAllEdits, &QAction::triggered, this, [ = ] { saveAllEdits(); } );
  connect( mActionRollbackEdits, &QAction::triggered, this, &QgisApp::rollbackEdits );
  connect( mActionRollbackAllEdits, &QAction::triggered, this, [ = ] { rollbackAllEdits(); } );
  connect( mActionCancelEdits, &QAction::triggered, this, [ = ] { cancelEdits(); } );
  connect( mActionCancelAllEdits, &QAction::triggered, this, [ = ] { cancelAllEdits(); } );
  connect( mActionLayerSaveAs, &QAction::triggered, this, [ = ] { saveAsFile(); } );
  connect( mActionSaveLayerDefinition, &QAction::triggered, this, &QgisApp::saveAsLayerDefinition );
  connect( mActionRemoveLayer, &QAction::triggered, this, &QgisApp::removeLayer );
  connect( mActionDuplicateLayer, &QAction::triggered, this, [ = ] { duplicateLayers(); } );
  connect( mActionSetLayerScaleVisibility, &QAction::triggered, this, &QgisApp::setLayerScaleVisibility );
  connect( mActionSetLayerCRS, &QAction::triggered, this, &QgisApp::setLayerCrs );
  connect( mActionSetProjectCRSFromLayer, &QAction::triggered, this, &QgisApp::setProjectCrsFromLayer );
  connect( mActionLayerProperties, &QAction::triggered, this, &QgisApp::layerProperties );
  connect( mActionLayerSubsetString, &QAction::triggered, this, qOverload<>( &QgisApp::layerSubsetString ) );
  connect( mActionAddToOverview, &QAction::triggered, this, &QgisApp::isInOverview );
  connect( mActionAddAllToOverview, &QAction::triggered, this, &QgisApp::addAllToOverview );
  connect( mActionRemoveAllFromOverview, &QAction::triggered, this, &QgisApp::removeAllFromOverview );
  connect( mActionShowAllLayers, &QAction::triggered, this, &QgisApp::showAllLayers );
  connect( mActionHideAllLayers, &QAction::triggered, this, &QgisApp::hideAllLayers );
  connect( mActionShowSelectedLayers, &QAction::triggered, this, &QgisApp::showSelectedLayers );
  connect( mActionHideSelectedLayers, &QAction::triggered, this, &QgisApp::hideSelectedLayers );
  connect( mActionToggleSelectedLayers, &QAction::triggered, this, &QgisApp::toggleSelectedLayers );
  connect( mActionToggleSelectedLayersIndependently, &QAction::triggered, this, &QgisApp::toggleSelectedLayersIndependently );
  connect( mActionHideDeselectedLayers, &QAction::triggered, this, &QgisApp::hideDeselectedLayers );

  // Plugin Menu Items

  connect( mActionManagePlugins, &QAction::triggered, this, &QgisApp::showPluginManager );
  connect( mActionShowPythonDialog, &QAction::triggered, this, &QgisApp::showPythonDialog );

  // Settings Menu Items

  connect( mActionToggleFullScreen, &QAction::triggered, this, &QgisApp::toggleFullScreen );
  connect( mActionTogglePanelsVisibility, &QAction::triggered, this, &QgisApp::togglePanelsVisibility );
  connect( mActionToggleMapOnly, &QAction::triggered, this, &QgisApp::toggleMapOnly );
  connect( mActionProjectProperties, &QAction::triggered, this, [ = ] {projectProperties( QString() );} );
  connect( mActionOptions, &QAction::triggered, this, &QgisApp::options );
  connect( mActionCustomProjection, &QAction::triggered, this, &QgisApp::customProjection );
  connect( mActionConfigureShortcuts, &QAction::triggered, this, &QgisApp::configureShortcuts );
  connect( mActionStyleManager, &QAction::triggered, this, &QgisApp::showStyleManager );
  connect( mActionCustomization, &QAction::triggered, this, &QgisApp::customize );

#ifdef Q_OS_MAC
  // Window Menu Items

  mActionWindowMinimize = new QAction( tr( "Minimize" ), this );
  mActionWindowMinimize->setShortcut( tr( "Ctrl+M", "Minimize Window" ) );
  mActionWindowMinimize->setStatusTip( tr( "Minimizes the active window to the dock" ) );
  connect( mActionWindowMinimize, SIGNAL( triggered() ), this, SLOT( showActiveWindowMinimized() ) );

  mActionWindowZoom = new QAction( tr( "Zoom" ), this );
  mActionWindowZoom->setStatusTip( tr( "Toggles between a predefined size and the window size set by the user" ) );
  connect( mActionWindowZoom, SIGNAL( triggered() ), this, SLOT( toggleActiveWindowMaximized() ) );

  mActionWindowAllToFront = new QAction( tr( "Bring All to Front" ), this );
  mActionWindowAllToFront->setStatusTip( tr( "Bring forward all open windows" ) );
  connect( mActionWindowAllToFront, SIGNAL( triggered() ), this, SLOT( bringAllToFront() ) );

  // list of open windows
  mWindowActions = new QActionGroup( this );
#endif

  // Vector edits menu
  QMenu *menuAllEdits = new QMenu( tr( "Current Edits" ), this );
  menuAllEdits->addAction( mActionSaveEdits );
  menuAllEdits->addAction( mActionRollbackEdits );
  menuAllEdits->addAction( mActionCancelEdits );
  menuAllEdits->addSeparator();
  menuAllEdits->addAction( mActionSaveAllEdits );
  menuAllEdits->addAction( mActionRollbackAllEdits );
  menuAllEdits->addAction( mActionCancelAllEdits );
  menuAllEdits->setObjectName( "AllEditsMenu" );
  mActionAllEdits->setMenu( menuAllEdits );

  // Raster toolbar items
  connect( mActionLocalHistogramStretch, &QAction::triggered, this, &QgisApp::localHistogramStretch );
  connect( mActionFullHistogramStretch, &QAction::triggered, this, &QgisApp::fullHistogramStretch );
  connect( mActionLocalCumulativeCutStretch, &QAction::triggered, this, &QgisApp::localCumulativeCutStretch );
  connect( mActionFullCumulativeCutStretch, &QAction::triggered, this, &QgisApp::fullCumulativeCutStretch );
  connect( mActionIncreaseBrightness, &QAction::triggered, this, &QgisApp::increaseBrightness );
  connect( mActionDecreaseBrightness, &QAction::triggered, this, &QgisApp::decreaseBrightness );
  connect( mActionIncreaseContrast, &QAction::triggered, this, &QgisApp::increaseContrast );
  connect( mActionDecreaseContrast, &QAction::triggered, this, &QgisApp::decreaseContrast );
  connect( mActionIncreaseGamma, &QAction::triggered, this, &QgisApp::increaseGamma );
  connect( mActionDecreaseGamma, &QAction::triggered, this, &QgisApp::decreaseGamma );

#ifdef HAVE_GEOREFERENCER
  connect( mActionShowGeoreferencer, &QAction::triggered, this, &QgisApp::showGeoreferencer );
#else
  delete mActionShowGeoreferencer;
  mActionShowGeoreferencer = nullptr;
#endif

  // Help Menu Items

#ifdef Q_OS_MAC
  mActionHelpContents->setShortcut( QString( "Ctrl+?" ) );
  mActionQgisHomePage->setShortcut( QString() );
  mActionReportaBug->setShortcut( QString() );
#endif

  mActionHelpContents->setEnabled( QFileInfo::exists( QgsApplication::pkgDataPath() + "/doc/index.html" ) );

  connect( mActionHelpContents, &QAction::triggered, this, &QgisApp::helpContents );
  connect( mActionHelpAPI, &QAction::triggered, this, &QgisApp::apiDocumentation );
  connect( mActionReportaBug, &QAction::triggered, this, &QgisApp::reportaBug );
  connect( mActionNeedSupport, &QAction::triggered, this, &QgisApp::supportProviders );
  connect( mActionQgisHomePage, &QAction::triggered, this, &QgisApp::helpQgisHomePage );
  connect( mActionCheckQgisVersion, &QAction::triggered, this, &QgisApp::checkQgisVersion );
  connect( mActionAbout, &QAction::triggered, this, &QgisApp::about );
  connect( mActionSponsors, &QAction::triggered, this, &QgisApp::sponsors );

  connect( mActionShowPinnedLabels, &QAction::toggled, this, &QgisApp::showPinnedLabels );
  connect( mActionShowUnplacedLabels, &QAction::toggled, this, [ = ]( bool active )
  {
    QgsLabelingEngineSettings engineSettings = QgsProject::instance()->labelingEngineSettings();
    engineSettings.setFlag( QgsLabelingEngineSettings::DrawUnplacedLabels, active );
    QgsProject::instance()->setLabelingEngineSettings( engineSettings );
    refreshMapCanvas( true );
  } );
  connect( QgsProject::instance(), &QgsProject::labelingEngineSettingsChanged, this, [ = ]
  {
    whileBlocking( mActionShowUnplacedLabels )->setChecked( QgsProject::instance()->labelingEngineSettings().testFlag( QgsLabelingEngineSettings::DrawUnplacedLabels ) );
  } );
  connect( mActionPinLabels, &QAction::triggered, this, &QgisApp::pinLabels );
  connect( mActionShowHideLabels, &QAction::triggered, this, &QgisApp::showHideLabels );
  connect( mActionMoveLabel, &QAction::triggered, this, &QgisApp::moveLabel );
  connect( mActionRotateLabel, &QAction::triggered, this, &QgisApp::rotateLabel );
  connect( mActionChangeLabelProperties, &QAction::triggered, this, &QgisApp::changeLabelProperties );

  connect( mActionDiagramProperties, &QAction::triggered, this, &QgisApp::diagramProperties );

  connect( mActionCreateAnnotationLayer, &QAction::triggered, this, &QgisApp::createAnnotationLayer );
  connect( mActionModifyAnnotation, &QAction::triggered, this, [ = ] {  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::AnnotationEdit ) ); } );
  connect( mMainAnnotationLayerProperties, &QAction::triggered, this, [ = ]
  {
    showLayerProperties( QgsProject::instance()->mainAnnotationLayer() );
  } );

  // we can't set the shortcut these actions, because we need to restrict their context to the canvas and it's children..
  for ( QWidget *widget :
        {
          static_cast< QWidget * >( mMapCanvas ),
          static_cast< QWidget * >( mLayerTreeView )
        } )
  {
    QShortcut *copyShortcut = new QShortcut( QKeySequence::Copy, widget );
    copyShortcut->setContext( Qt::WidgetWithChildrenShortcut );
    connect( copyShortcut, &QShortcut::activated, this, [ = ] { copySelectionToClipboard(); } );

    QShortcut *cutShortcut = new QShortcut( QKeySequence::Cut, widget );
    cutShortcut->setContext( Qt::WidgetWithChildrenShortcut );
    connect( cutShortcut, &QShortcut::activated, this, [ = ] { cutSelectionToClipboard(); } );

    QShortcut *pasteShortcut = new QShortcut( QKeySequence::Paste, widget );
    pasteShortcut->setContext( Qt::WidgetWithChildrenShortcut );
    connect( pasteShortcut, &QShortcut::activated, this, [ = ] { pasteFromClipboard(); } );

    QShortcut *selectAllShortcut = new QShortcut( QKeySequence::SelectAll, widget );
    selectAllShortcut->setContext( Qt::WidgetWithChildrenShortcut );
    connect( selectAllShortcut, &QShortcut::activated, this, &QgisApp::selectAll );
  }

#ifndef HAVE_POSTGRESQL
  delete mActionAddPgLayer;
  mActionAddPgLayer = 0;
#endif

#ifndef HAVE_ORACLE
  delete mActionAddOracleLayer;
  mActionAddOracleLayer = nullptr;
#endif

#ifndef HAVE_HANA
  delete mActionAddHanaLayer;
  mActionAddHanaLayer = nullptr;
#endif

}

void QgisApp::showStyleManager()
{
  QgsGui::windowManager()->openStandardDialog( QgsWindowManagerInterface::DialogStyleManager );
}

void QgisApp::initPythonConsoleOptions()
{
  QgsPythonRunner::run( QStringLiteral( "import console" ) );
  QgsPythonRunner::run( QStringLiteral( "console.init_options_widget()" ) );
}

void QgisApp::showPythonDialog()
{
#ifdef WITH_BINDINGS
  if ( !mPythonUtils || !mPythonUtils->isEnabled() )
    return;

  bool res = mPythonUtils->runString(
               "import console\n"
               "console.show_console()\n", tr( "Failed to open Python console:" ), false );

  if ( !res )
  {
    QString className, text;
    mPythonUtils->getError( className, text );
    visibleMessageBar()->pushMessage( tr( "Error" ), tr( "Failed to open Python console:" ) + '\n' + className + ": " + text, Qgis::MessageLevel::Warning );
  }
#ifdef Q_OS_MAC
  else
  {
    addWindow( mActionShowPythonDialog );
  }
#endif
#endif
}

void QgisApp::createActionGroups()
{
  //
  // Map Tool Group
  mMapToolGroup = new QActionGroup( this );
  mMapToolGroup->addAction( mActionPan );
  mMapToolGroup->addAction( mActionZoomIn );
  mMapToolGroup->addAction( mActionZoomOut );
  mMapToolGroup->addAction( mActionIdentify );
  mMapToolGroup->addAction( mActionFeatureAction );
  mMapToolGroup->addAction( mActionSelectFeatures );
  mMapToolGroup->addAction( mActionSelectPolygon );
  mMapToolGroup->addAction( mActionSelectFreehand );
  mMapToolGroup->addAction( mActionSelectRadius );
  mMapToolGroup->addAction( mActionDeselectAll );
  mMapToolGroup->addAction( mActionDeselectActiveLayer );
  mMapToolGroup->addAction( mActionSelectAll );
  mMapToolGroup->addAction( mActionReselect );
  mMapToolGroup->addAction( mActionInvertSelection );
  mMapToolGroup->addAction( mActionMeasure );
  mMapToolGroup->addAction( mActionMeasureArea );
  mMapToolGroup->addAction( mActionMeasureAngle );
  mMapToolGroup->addAction( mActionMeasureBearing );
  mMapToolGroup->addAction( mActionAddFeature );
  mMapToolGroup->addAction( mActionMoveFeature );
  mMapToolGroup->addAction( mActionMoveFeatureCopy );
  mMapToolGroup->addAction( mActionRotateFeature );
  mMapToolGroup->addAction( mActionScaleFeature );
  mMapToolGroup->addAction( mActionOffsetCurve );
  mMapToolGroup->addAction( mActionReshapeFeatures );
  mMapToolGroup->addAction( mActionSplitFeatures );
  mMapToolGroup->addAction( mActionSplitParts );
  mMapToolGroup->addAction( mActionDeleteSelected );
  mMapToolGroup->addAction( mActionAddRing );
  mMapToolGroup->addAction( mActionFillRing );
  mMapToolGroup->addAction( mActionAddPart );
  mMapToolGroup->addAction( mActionSimplifyFeature );
  mMapToolGroup->addAction( mActionDeleteRing );
  mMapToolGroup->addAction( mActionDeletePart );
  mMapToolGroup->addAction( mActionMergeFeatures );
  mMapToolGroup->addAction( mActionMergeFeatureAttributes );
  mMapToolGroup->addAction( mActionVertexTool );
  mMapToolGroup->addAction( mActionVertexToolActiveLayer );
  mMapToolGroup->addAction( mActionRotatePointSymbols );
  mMapToolGroup->addAction( mActionOffsetPointSymbol );
  mMapToolGroup->addAction( mActionPinLabels );
  mMapToolGroup->addAction( mActionShowHideLabels );
  mMapToolGroup->addAction( mActionMoveLabel );
  mMapToolGroup->addAction( mActionRotateLabel );
  mMapToolGroup->addAction( mActionChangeLabelProperties );
  mMapToolGroup->addAction( mActionReverseLine );
  mMapToolGroup->addAction( mActionTrimExtendFeature );
  mMapToolGroup->addAction( mActionModifyAnnotation );

  //
  // Preview Modes Group
  QActionGroup *mPreviewGroup = new QActionGroup( this );
  mPreviewGroup->setExclusive( true );
  mActionPreviewModeOff->setActionGroup( mPreviewGroup );
  mActionPreviewModeMono->setActionGroup( mPreviewGroup );
  mActionPreviewModeGrayscale->setActionGroup( mPreviewGroup );
  mActionPreviewProtanope->setActionGroup( mPreviewGroup );
  mActionPreviewDeuteranope->setActionGroup( mPreviewGroup );
  mActionPreviewTritanope->setActionGroup( mPreviewGroup );
}

void QgisApp::setAppStyleSheet( const QString &stylesheet )
{
  // avoid crash on stylesheet change -- see https://bugreports.qt.io/browse/QTBUG-69204
  static bool sOnce = false;
  if ( sOnce )
    return;
  sOnce = true;

  setStyleSheet( stylesheet );

  // cascade styles to any current layout designers
  const auto constMLayoutDesignerDialogs = mLayoutDesignerDialogs;
  for ( QgsLayoutDesignerDialog *d : constMLayoutDesignerDialogs )
  {
    d->setStyleSheet( stylesheet );
  }

  if ( mpMaptip )
  {
    mpMaptip->applyFontSettings();
  }
}

void QgisApp::createMenus()
{
  /*
   * The User Interface Guidelines for each platform specify different locations
   * for the following items.
   *
   * Custom CRS, Options:
   * Gnome - bottom of Edit menu
   * Mac - Application menu (moved automatically by Qt)
   * Kde, Win - Settings menu (Win should use Tools menu but we don't have one)
   *
   * Panel and Toolbar submenus, Toggle Full Screen:
   * Gnome, Mac, Win - View menu
   * Kde - Settings menu
   *
   * For Mac, About and Exit are also automatically moved by Qt to the Application menu.
   */

  // Layer menu

  // Panel and Toolbar Submenus
  mPanelMenu = new QMenu( tr( "Panels" ), this );
  mPanelMenu->setObjectName( QStringLiteral( "mPanelMenu" ) );
  mToolbarMenu = new QMenu( tr( "Toolbars" ), this );
  mToolbarMenu->setObjectName( QStringLiteral( "mToolbarMenu" ) );

  // Get platform for menu layout customization (Gnome, Kde, Mac, Win)
  QDialogButtonBox::ButtonLayout layout =
    QDialogButtonBox::ButtonLayout( style()->styleHint( QStyle::SH_DialogButtonLayout, nullptr, this ) );

  // Connect once for the entire submenu.
  connect( mRecentProjectsMenu, &QMenu::triggered, this, static_cast < void ( QgisApp::* )( QAction *action ) >( &QgisApp::openProject ) );
  connect( mProjectFromTemplateMenu, &QMenu::triggered,
           this, &QgisApp::fileNewFromTemplateAction );


  // View Menu

  if ( layout != QDialogButtonBox::KdeLayout )
  {
    mViewMenu->addSeparator();
    mViewMenu->addMenu( mPanelMenu );
    mViewMenu->addMenu( mToolbarMenu );
    mViewMenu->addAction( mActionToggleFullScreen );
    mViewMenu->addAction( mActionTogglePanelsVisibility );
    mViewMenu->addAction( mActionToggleMapOnly );
  }
  else
  {
    // on the top of the settings menu
    QAction *before = mSettingsMenu->actions().at( 0 );
    mSettingsMenu->insertMenu( before, mPanelMenu );
    mSettingsMenu->insertMenu( before, mToolbarMenu );
    mSettingsMenu->insertAction( before, mActionToggleFullScreen );
    mSettingsMenu->insertAction( before, mActionTogglePanelsVisibility );
    mSettingsMenu->insertAction( before, mActionToggleMapOnly );
    mSettingsMenu->insertSeparator( before );
  }

#ifdef Q_OS_MAC

  // keep plugins from hijacking About and Preferences application menus
  // these duplicate actions will be moved to application menus by Qt
  mProjectMenu->addAction( mActionAbout );
  QAction *actionPrefs = new QAction( tr( "Preferences…" ), this );
  // actionPrefs->setMenuRole( QAction::PreferencesRole );  // SG add so that it does not hijack the preferences menu
  actionPrefs->setIcon( mActionOptions->icon() );
  connect( actionPrefs, &QAction::triggered, this, &QgisApp::options );
  mProjectMenu->addAction( actionPrefs );

  // SG add start
  // Window Menu

  //mWindowMenu = new QMenu( tr( "Window" ), this );

  // mWindowMenu->addAction( mActionWindowMinimize );
  //mWindowMenu->addAction( mActionWindowZoom );
  //mWindowMenu->addSeparator();

  //  mWindowMenu->addAction( mActionWindowAllToFront );
  //  mWindowMenu->addSeparator();

  //  // insert before Help menu, as per Mac OS convention
  //  menuBar()->insertMenu( mHelpMenu->menuAction(), mWindowMenu );
  // SG add end
#endif

  // Database Menu
  // don't add it yet, wait for a plugin
  mDatabaseMenu = new QMenu( tr( "&Database" ), menuBar() );
  mDatabaseMenu->setObjectName( QStringLiteral( "mDatabaseMenu" ) );
  // Web Menu
  // don't add it yet, wait for a plugin
  mWebMenu = new QMenu( tr( "&Web" ), menuBar() );
  mWebMenu->setObjectName( QStringLiteral( "mWebMenu" ) );

  // createProfileMenu(); // SG add
}

void QgisApp::refreshProfileMenu()
{
  if ( !mConfigMenu )
    return;

  mConfigMenu->clear();
  QgsUserProfile *profile = userProfileManager()->userProfile();
  QString activeName = profile->name();
  mConfigMenu->setTitle( tr( "&User Profiles" ) );

  QActionGroup *profileGroup = new QActionGroup( mConfigMenu );
  profileGroup->setExclusive( true );

  const auto constAllProfiles = userProfileManager()->allProfiles();
  for ( const QString &name : constAllProfiles )
  {
    std::unique_ptr< QgsUserProfile > namedProfile( userProfileManager()->profileForName( name ) );
    QAction *action = new QAction( namedProfile->icon(), namedProfile->alias(), profileGroup );
    action->setToolTip( namedProfile->folder() );
    action->setCheckable( true );
    action->setObjectName( "mActionProfile_" + namedProfile->alias() );
    mConfigMenu->addAction( action );

    if ( name == activeName )
    {
      action->setChecked( true );
    }
    else
    {
      connect( action, &QAction::triggered, this, [this, name]()
      {
        userProfileManager()->loadUserProfile( name );
      } );
    }
  }

  mConfigMenu->addSeparator( );

  QAction *openProfileFolderAction = mConfigMenu->addAction( tr( "Open Active Profile Folder" ) );
  openProfileFolderAction->setObjectName( "mActionOpenActiveProfileFolder" );
  connect( openProfileFolderAction, &QAction::triggered, this, [this]()
  {
    QDesktopServices::openUrl( QUrl::fromLocalFile( userProfileManager()->userProfile()->folder() ) );
  } );

  QAction *newProfileAction = mConfigMenu->addAction( tr( "New Profile…" ) );
  newProfileAction->setObjectName( "mActionNewProfile" );
  connect( newProfileAction, &QAction::triggered, this, &QgisApp::newProfile );
}

void QgisApp::createProfileMenu()
{
  mConfigMenu = new QMenu( this );
  mConfigMenu->setObjectName( "mUserProfileMenu" );

  settingsMenu()->insertMenu( settingsMenu()->actions().first(), mConfigMenu );

  refreshProfileMenu();
}

void QgisApp::createToolBars()
{
  QgsSettings settings;
  // QSize myIconSize ( 32,32 ); //large icons
  // Note: we need to set each object name to ensure that
  // qmainwindow::saveState and qmainwindow::restoreState
  // work properly

  QList<QToolBar *> toolbarMenuToolBars;
  toolbarMenuToolBars << mFileToolBar
                      << mDataSourceManagerToolBar
                      << mLayerToolBar
                      << mDigitizeToolBar
                      << mAdvancedDigitizeToolBar
                      << mShapeDigitizeToolBar
                      << mMapNavToolBar
                      << mAttributesToolBar
                      << mSelectionToolBar
                      << mPluginToolBar
                      << mHelpToolBar
                      << mRasterToolBar
                      << mVectorToolBar
                      << mDatabaseToolBar
                      << mWebToolBar
                      << mLabelToolBar
                      << mSnappingToolBar
                      << mMeshToolBar
                      << mAnnotationsToolBar;

  mSnappingWidget = new QgsSnappingWidget( QgsProject::instance(), mMapCanvas, mSnappingToolBar );
  mSnappingWidget->setObjectName( QStringLiteral( "mSnappingWidget" ) );
  connect( mSnappingWidget, &QgsSnappingWidget::snappingConfigChanged, QgsProject::instance(), [ = ] { QgsProject::instance()->setSnappingConfig( mSnappingWidget->config() ); } );
  mSnappingToolBar->addWidget( mSnappingWidget );

  mTracer = new QgsMapCanvasTracer( mMapCanvas, messageBar() );
  mTracer->setActionEnableTracing( mSnappingWidget->enableTracingAction() );
  mTracer->setActionEnableSnapping( mSnappingWidget->enableSnappingAction() );
  connect( mSnappingWidget->tracingOffsetSpinBox(),
           static_cast< void ( QgsDoubleSpinBox::* )( double ) >( &QgsDoubleSpinBox::valueChanged ),
  this, [ = ]( double v ) { mTracer->setOffset( v ); } );

  mDigitizingTechniqueManager->setupToolBars();


  QList<QAction *> toolbarMenuActions;
  // Set action names so that they can be used in customization
  const auto constToolbarMenuToolBars = toolbarMenuToolBars;
  for ( QToolBar *toolBar : constToolbarMenuToolBars )
  {
    toolBar->toggleViewAction()->setObjectName( "mActionToggle" + toolBar->objectName().mid( 1 ) );
    toolbarMenuActions << toolBar->toggleViewAction();
  }

  // sort actions in toolbar menu
  std::sort( toolbarMenuActions.begin(), toolbarMenuActions.end(), cmpByText_ );

  mToolbarMenu->addActions( toolbarMenuActions );

  // advanced selection tool button
  QToolButton *bt = new QToolButton( mSelectionToolBar );
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->addAction( mActionSelectByForm );
  bt->addAction( mActionSelectByExpression );
  bt->addAction( mActionSelectAll );
  bt->addAction( mActionInvertSelection );

  QAction *defAdvancedSelectionAction = mActionSelectByForm;
  switch ( settings.value( QStringLiteral( "UI/selectionTool" ), 0 ).toInt() )
  {
    case 0:
      defAdvancedSelectionAction = mActionSelectByForm;
      break;
    case 1:
      defAdvancedSelectionAction = mActionSelectByExpression;
      break;
    case 2:
      defAdvancedSelectionAction = mActionSelectAll;
      break;
    case 3:
      defAdvancedSelectionAction = mActionInvertSelection;
      break;
  }
  bt->setDefaultAction( defAdvancedSelectionAction );
  QAction *advancedSelectionAction = mSelectionToolBar->insertWidget( mActionOpenTable, bt );
  advancedSelectionAction->setObjectName( QStringLiteral( "ActionSelection" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  // mouse select tool button
  bt = new QToolButton( mSelectionToolBar );
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->addAction( mActionSelectFeatures );
  bt->addAction( mActionSelectPolygon );
  bt->addAction( mActionSelectFreehand );
  bt->addAction( mActionSelectRadius );

  QAction *defMouseSelectAction = mActionSelectFeatures;
  switch ( settings.value( QStringLiteral( "UI/selectTool" ), 1 ).toInt() )
  {
    case 1:
      defMouseSelectAction = mActionSelectFeatures;
      break;
    case 2:
      defMouseSelectAction = mActionSelectRadius;
      break;
    case 3:
      defMouseSelectAction = mActionSelectPolygon;
      break;
    case 4:
      defMouseSelectAction = mActionSelectFreehand;
      break;
  }
  bt->setDefaultAction( defMouseSelectAction );
  QAction *mouseSelectionAction = mSelectionToolBar->insertWidget( advancedSelectionAction, bt );
  mouseSelectionAction->setObjectName( QStringLiteral( "ActionSelect" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  // deselection tool button
  bt = new QToolButton( mSelectionToolBar );
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->addAction( mActionDeselectAll );
  bt->addAction( mActionDeselectActiveLayer );

  QAction *defDeselectionAction = mActionDeselectAll;
  switch ( settings.value( QStringLiteral( "UI/deselectionTool" ), 0 ).toInt() )
  {
    case 0:
      defDeselectionAction = mActionDeselectAll;
      break;
    case 1:
      defDeselectionAction = mActionDeselectActiveLayer;
      break;
  }
  bt->setDefaultAction( defDeselectionAction );
  QAction *deselectionAction = mSelectionToolBar->insertWidget( mActionOpenTable, bt );
  deselectionAction->setObjectName( QStringLiteral( "ActionDeselection" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  // feature action tool button
  bt = new QToolButton( mAttributesToolBar );
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->setDefaultAction( mActionFeatureAction );
  mFeatureActionMenu = new QMenu( bt );
  connect( mFeatureActionMenu, &QMenu::triggered, this, &QgisApp::updateDefaultFeatureAction );
  connect( mFeatureActionMenu, &QMenu::triggered, this, &QgisApp::doFeatureAction );
  connect( mFeatureActionMenu, &QMenu::aboutToShow, this, &QgisApp::refreshFeatureActions );
  bt->setMenu( mFeatureActionMenu );
  QAction *featureActionAction = mAttributesToolBar->insertWidget( mouseSelectionAction, bt );
  featureActionAction->setObjectName( QStringLiteral( "ActionFeatureAction" ) );



  // open table tool button

  bt = new QToolButton( mAttributesToolBar );
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->addAction( mActionOpenTable );
  bt->addAction( mActionOpenTableSelected );
  bt->addAction( mActionOpenTableVisible );
  bt->addAction( mActionOpenTableEdited );

  QAction *defOpenTableAction = mActionOpenTable;
  switch ( settings.value( QStringLiteral( "UI/openTableTool" ), 0 ).toInt() )
  {
    case 0:
      defOpenTableAction = mActionOpenTable;
      break;
    case 1:
      defOpenTableAction = mActionOpenTableSelected;
      break;
    case 2:
      defOpenTableAction = mActionOpenTableVisible;
      break;
    case 3:
      defOpenTableAction = mActionOpenTableEdited;
      break;
  }
  bt->setDefaultAction( defOpenTableAction );
  QAction *openTableAction = mAttributesToolBar->insertWidget( mActionMapTips, bt );
  openTableAction->setObjectName( QStringLiteral( "ActionOpenTable" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );



  // measure tool button

  bt = new QToolButton( mAttributesToolBar );
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->addAction( mActionMeasure );
  bt->addAction( mActionMeasureArea );
  bt->addAction( mActionMeasureBearing );
  bt->addAction( mActionMeasureAngle );

  QAction *defMeasureAction = mActionMeasure;
  switch ( settings.value( QStringLiteral( "UI/measureTool" ), 0 ).toInt() )
  {
    case 0:
      defMeasureAction = mActionMeasure;
      break;
    case 1:
      defMeasureAction = mActionMeasureArea;
      break;
    case 2:
      defMeasureAction = mActionMeasureBearing;
      break;
    case 3:
      defMeasureAction = mActionMeasureAngle;
      break;
  }
  bt->setDefaultAction( defMeasureAction );
  QAction *measureAction = mAttributesToolBar->insertWidget( mActionMapTips, bt );
  measureAction->setObjectName( QStringLiteral( "ActionMeasure" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  // vector layer edits tool buttons
  QToolButton *tbAllEdits = qobject_cast<QToolButton *>( mDigitizeToolBar->widgetForAction( mActionAllEdits ) );
  tbAllEdits->setPopupMode( QToolButton::InstantPopup );

  // new layer tool button

  bt = new QToolButton();
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->addAction( mActionNewVectorLayer );
#ifdef HAVE_SPATIALITE
  bt->addAction( mActionNewSpatiaLiteLayer );
#endif
  bt->addAction( mActionNewGeoPackageLayer );
  bt->addAction( mActionNewMemoryLayer );

  QAction *defNewLayerAction = mActionNewVectorLayer;
  switch ( settings.value( QStringLiteral( "UI/defaultNewLayer" ), 1 ).toInt() )
  {
    case 0:
      defNewLayerAction = mActionNewSpatiaLiteLayer;
      break;
    case 1:
      defNewLayerAction = mActionNewVectorLayer;
      break;
    case 2:
      defNewLayerAction = mActionNewMemoryLayer;
      break;
    case 3:
      defNewLayerAction = mActionNewGeoPackageLayer;
      break;
  }
  bt->setDefaultAction( defNewLayerAction );
  QAction *newLayerAction = mLayerToolBar->addWidget( bt );

  newLayerAction->setObjectName( QStringLiteral( "ActionNewLayer" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  // add db layer button
  bt = new QToolButton();
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  if ( mActionAddPgLayer )
    bt->addAction( mActionAddPgLayer );
  if ( mActionAddMssqlLayer )
    bt->addAction( mActionAddMssqlLayer );
  if ( mActionAddOracleLayer )
    bt->addAction( mActionAddOracleLayer );
  if ( mActionAddHanaLayer )
    bt->addAction( mActionAddHanaLayer );
  QAction *defAddDbLayerAction = mActionAddPgLayer;
  switch ( settings.value( QStringLiteral( "UI/defaultAddDbLayerAction" ), 0 ).toInt() )
  {
    case 0:
      defAddDbLayerAction = mActionAddPgLayer;
      break;
    case 1:
      defAddDbLayerAction = mActionAddMssqlLayer;
      break;
    case 2:
      defAddDbLayerAction = mActionAddOracleLayer;
      break;
    case 3:
      defAddDbLayerAction = mActionAddHanaLayer;
      break;
  }
  if ( defAddDbLayerAction )
    bt->setDefaultAction( defAddDbLayerAction );
  QAction *addDbLayerAction = mLayerToolBar->insertWidget( mActionAddWmsLayer, bt );
  addDbLayerAction->setObjectName( QStringLiteral( "ActionAddDbLayer" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  QLayout *layout = mLayerToolBar->layout();
  for ( int i = 0; i < layout->count(); ++i )
  {
    layout->itemAt( i )->setAlignment( Qt::AlignLeft );
  }

  // Cad toolbar
  mAdvancedDigitizeToolBar->insertAction( mAdvancedDigitizeToolBar->actions().at( 0 ), mAdvancedDigitizingDockWidget->enableAction() );

  // move feature tool button
  QToolButton *moveFeatureButton = new QToolButton( mAdvancedDigitizeToolBar );
  moveFeatureButton->setPopupMode( QToolButton::MenuButtonPopup );
  moveFeatureButton->addAction( mActionMoveFeature );
  moveFeatureButton->addAction( mActionMoveFeatureCopy );
  QAction *defAction = mActionMoveFeature;
  switch ( settings.value( QStringLiteral( "UI/defaultMoveTool" ), 0 ).toInt() )
  {
    case 0:
      defAction = mActionMoveFeature;
      break;
    case 1:
      defAction = mActionMoveFeatureCopy;
      break;
  }
  moveFeatureButton->setDefaultAction( defAction );
  QAction *moveToolAction = mAdvancedDigitizeToolBar->insertWidget( mActionRotateFeature, moveFeatureButton );
  moveToolAction->setObjectName( QStringLiteral( "ActionMoveFeatureTool" ) );
  connect( moveFeatureButton, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  // vertex tool button
  QToolButton *vertexToolButton = new QToolButton( mDigitizeToolBar );
  vertexToolButton->setPopupMode( QToolButton::MenuButtonPopup );
  vertexToolButton->addAction( mActionVertexTool );
  vertexToolButton->addAction( mActionVertexToolActiveLayer );
  QAction *defActionVertexTool = mActionVertexTool;
  switch ( settings.enumValue( QStringLiteral( "UI/defaultVertexTool" ), QgsVertexTool::ActiveLayer ) )
  {
    case QgsVertexTool::AllLayers:
      defActionVertexTool = mActionVertexTool;
      break;
    case QgsVertexTool::ActiveLayer:
      defActionVertexTool = mActionVertexToolActiveLayer;
      break;
  }

  QAction *showVertexEditorAction = new QAction( tr( "Show Vertex Editor" ), this );
  showVertexEditorAction->setCheckable( true );
  showVertexEditorAction->setProperty( "no_default_action", true );
  vertexToolButton->addAction( showVertexEditorAction );
  mVertexEditorDock->setToggleVisibilityAction( showVertexEditorAction );

  vertexToolButton->setDefaultAction( defActionVertexTool );
  QAction *actionVertexTool = mDigitizeToolBar->insertWidget( mActionMultiEditAttributes, vertexToolButton );
  actionVertexTool->setObjectName( QStringLiteral( "ActionVertexTool" ) );
  connect( vertexToolButton, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  bt = new QToolButton();
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->addAction( mActionRotatePointSymbols );
  bt->addAction( mActionOffsetPointSymbol );

  QAction *defPointSymbolAction = mActionRotatePointSymbols;
  switch ( settings.value( QStringLiteral( "UI/defaultPointSymbolAction" ), 0 ).toInt() )
  {
    case 0:
      defPointSymbolAction = mActionRotatePointSymbols;
      break;
    case 1:
      defPointSymbolAction = mActionOffsetPointSymbol;
      break;
  }
  bt->setDefaultAction( defPointSymbolAction );
  QAction *pointSymbolAction = mAdvancedDigitizeToolBar->addWidget( bt );
  pointSymbolAction->setObjectName( QStringLiteral( "ActionPointSymbolTools" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );

  QgsMapToolEditMeshFrame *editMeshMapTool = qobject_cast<QgsMapToolEditMeshFrame *>( mMapTools->mapTool( QgsAppMapTools::EditMeshFrame ) );
  if ( editMeshMapTool )
  {
    mMeshToolBar->addAction( editMeshMapTool->digitizeAction() );

    QToolButton *meshSelectToolButton = new QToolButton();
    meshSelectToolButton->setPopupMode( QToolButton::MenuButtonPopup );
    QList<QAction *> selectActions = editMeshMapTool->selectActions();
    for ( QAction *selectAction : selectActions )
    {
      meshSelectToolButton->addAction( selectAction );
      connect( selectAction, &QAction::triggered, meshSelectToolButton, [selectAction, meshSelectToolButton]
      {
        meshSelectToolButton->setDefaultAction( selectAction );
      } );
    }

    meshSelectToolButton->setDefaultAction( editMeshMapTool->defaultSelectActions() );
    mMeshToolBar->addWidget( meshSelectToolButton );

    mMeshToolBar->addAction( ( editMeshMapTool->transformAction() ) );

    QToolButton *meshForceByLinesToolButton = new QToolButton();
    meshForceByLinesToolButton->setPopupMode( QToolButton::MenuButtonPopup );
    QMenu *meshForceByLineMenu = new QMenu( meshForceByLinesToolButton );

    //meshForceByLineMenu->addActions( editMeshMapTool->forceByLinesActions() );
    meshForceByLinesToolButton->setDefaultAction( editMeshMapTool->defaultForceAction() );
    meshForceByLineMenu->addSeparator();
    meshForceByLineMenu->addAction( editMeshMapTool->forceByLineWidgetActionSettings() );
    meshForceByLinesToolButton->setMenu( meshForceByLineMenu );
    mMeshToolBar->addWidget( meshForceByLinesToolButton );

    for ( QAction *mapToolAction : editMeshMapTool->mapToolActions() )
      mMapToolGroup->addAction( mapToolAction );

    mMeshMenu->addAction( editMeshMapTool->reindexAction() );
  }

  QToolButton *annotationLayerToolButton = new QToolButton();
  annotationLayerToolButton->setPopupMode( QToolButton::MenuButtonPopup );
  QMenu *annotationLayerMenu = new QMenu( annotationLayerToolButton );
  annotationLayerMenu->addAction( mActionCreateAnnotationLayer );
  annotationLayerMenu->addAction( mMainAnnotationLayerProperties );
  annotationLayerToolButton->setMenu( annotationLayerMenu );
  annotationLayerToolButton->setDefaultAction( mActionCreateAnnotationLayer );
  mAnnotationsToolBar->insertWidget( mAnnotationsToolBar->actions().at( 0 ), annotationLayerToolButton );

  // Registered annotation items will be inserted before this separator
  mAnnotationsItemInsertBefore = mAnnotationsToolBar->addSeparator();

  bt = new QToolButton();
  bt->setPopupMode( QToolButton::MenuButtonPopup );
  bt->addAction( mActionTextAnnotation );
  bt->addAction( mActionFormAnnotation );
  bt->addAction( mActionHtmlAnnotation );
  bt->addAction( mActionSvgAnnotation );
  bt->addAction( mActionAnnotation );

  QAction *defAnnotationAction = mActionTextAnnotation;
  switch ( settings.value( QStringLiteral( "UI/annotationTool" ), 0 ).toInt() )
  {
    case 0:
      defAnnotationAction = mActionTextAnnotation;
      break;
    case 1:
      defAnnotationAction = mActionFormAnnotation;
      break;
    case 2:
      defAnnotationAction = mActionHtmlAnnotation;
      break;
    case 3:
      defAnnotationAction = mActionSvgAnnotation;
      break;
    case 4:
      defAnnotationAction = mActionAnnotation;
      break;
  }
  bt->setDefaultAction( defAnnotationAction );
  QAction *annotationAction = mAnnotationsToolBar->addWidget( bt );
  annotationAction->setObjectName( QStringLiteral( "ActionAnnotation" ) );
  connect( bt, &QToolButton::triggered, this, &QgisApp::toolButtonActionTriggered );
}

void QgisApp::createStatusBar()
{
  //remove borders from children under Windows
  statusBar()->setStyleSheet( QStringLiteral( "QStatusBar::item {border: none;}" ) );

  // Drop the font size in the status bar by a couple of points
  QFont statusBarFont = font();
  int fontSize = statusBarFont.pointSize();
#ifdef Q_OS_WIN
  fontSize = std::max( fontSize - 1, 8 ); // bit less on windows, due to poor rendering of small point sizes
#else
  fontSize = std::max( fontSize - 2, 6 );
#endif
  statusBarFont.setPointSize( fontSize );
  statusBar()->setFont( statusBarFont );

  mStatusBar = new QgsStatusBar();
  mStatusBar->setParentStatusBar( QMainWindow::statusBar() );
  mStatusBar->setFont( statusBarFont );

  statusBar()->addPermanentWidget( mStatusBar, 10 );

  // Add a panel to the status bar for the scale, coords and progress
  // And also rendering suppression checkbox
  mProgressBar = new QProgressBar( mStatusBar );
  mProgressBar->setObjectName( QStringLiteral( "mProgressBar" ) );
  mProgressBar->setMaximumWidth( 100 );
  mProgressBar->setMaximumHeight( 18 );
  mProgressBar->hide();
  mStatusBar->addPermanentWidget( mProgressBar, 1 );

  connect( mMapCanvas, &QgsMapCanvas::renderStarting, this, &QgisApp::canvasRefreshStarted );
  connect( mMapCanvas, &QgsMapCanvas::mapCanvasRefreshed, this, &QgisApp::canvasRefreshFinished );

  // SG change// render suppression status bar widget
  //  mTaskManagerWidget = new QgsTaskManagerStatusBarWidget( QgsApplication::taskManager(), mStatusBar );
  //  mTaskManagerWidget->setFont( statusBarFont );
  //  mStatusBar->addPermanentWidget( mTaskManagerWidget, 0 );

  // SG change
  //coords status bar widget
  //  mCoordsEdit = new QgsStatusBarCoordinatesWidget( mStatusBar );
  //  mCoordsEdit->setObjectName( QStringLiteral( "mCoordsEdit" ) );
  //  mCoordsEdit->setMapCanvas( mMapCanvas );
  //  mCoordsEdit->setFont( statusBarFont );
  //  mStatusBar->addPermanentWidget( mCoordsEdit, 0 );

  mScaleWidget = new QgsStatusBarScaleWidget( mMapCanvas, mStatusBar );
  mScaleWidget->setObjectName( QStringLiteral( "mScaleWidget" ) );
  mScaleWidget->setFont( statusBarFont );
  mStatusBar->addPermanentWidget( mScaleWidget, 0 );

  // zoom widget
  mMagnifierWidget = new QgsStatusBarMagnifierWidget( mStatusBar );
  mMagnifierWidget->setObjectName( QStringLiteral( "mMagnifierWidget" ) );
  mMagnifierWidget->setFont( statusBarFont );
  connect( mMapCanvas, &QgsMapCanvas::magnificationChanged, mMagnifierWidget, &QgsStatusBarMagnifierWidget::updateMagnification );
  connect( mMapCanvas, &QgsMapCanvas::scaleLockChanged, mMagnifierWidget, &QgsStatusBarMagnifierWidget::updateScaleLock );
  connect( mMagnifierWidget, &QgsStatusBarMagnifierWidget::magnificationChanged, mMapCanvas, [ = ]( double factor ) { mMapCanvas->setMagnificationFactor( factor ); } );
  connect( mMagnifierWidget, &QgsStatusBarMagnifierWidget::scaleLockChanged, mMapCanvas, &QgsMapCanvas::setScaleLocked );
  mMagnifierWidget->updateMagnification( QSettings().value( QStringLiteral( "/qgis/magnifier_factor_default" ), 1.0 ).toDouble() );
  mStatusBar->addPermanentWidget( mMagnifierWidget, 0 );

  // add a widget to show/set current rotation
  mRotationLabel = new QLabel( QString(), mStatusBar );
  mRotationLabel->setObjectName( QStringLiteral( "mRotationLabel" ) );
  mRotationLabel->setFont( statusBarFont );
  mRotationLabel->setMinimumWidth( 10 );
  //mRotationLabel->setMaximumHeight( 20 );
  mRotationLabel->setMargin( 3 );
  mRotationLabel->setAlignment( Qt::AlignCenter );
  mRotationLabel->setFrameStyle( QFrame::NoFrame );
  mRotationLabel->setText( tr( "Rotation" ) );
  mRotationLabel->setToolTip( tr( "Current clockwise map rotation in degrees" ) );
  mStatusBar->addPermanentWidget( mRotationLabel, 0 );

  mRotationEdit = new QgsDoubleSpinBox( mStatusBar );
  mRotationEdit->setObjectName( QStringLiteral( "mRotationEdit" ) );
  mRotationEdit->setClearValue( 0.0 );
  mRotationEdit->setKeyboardTracking( false );
  mRotationEdit->setMaximumWidth( 120 );
  mRotationEdit->setDecimals( 1 );
  mRotationEdit->setRange( -360.0, 360.0 );
  mRotationEdit->setWrapping( true );
  mRotationEdit->setSingleStep( 5.0 );
  mRotationEdit->setFont( statusBarFont );
  mRotationEdit->setSuffix( tr( " °" ) );
  mRotationEdit->setToolTip( tr( "Current clockwise map rotation in degrees" ) );
  mStatusBar->addPermanentWidget( mRotationEdit, 0 );
  connect( mRotationEdit, static_cast < void ( QgsDoubleSpinBox::* )( double ) > ( &QgsDoubleSpinBox::valueChanged ), this, &QgisApp::userRotation );

  showRotation();

  // render suppression status bar widget
  // SG change
  //  mRenderSuppressionCBox = new QCheckBox( tr( "Render" ), mStatusBar );
  //  mRenderSuppressionCBox->setObjectName( QStringLiteral( "mRenderSuppressionCBox" ) );
  //  mRenderSuppressionCBox->setChecked( true );
  //  mRenderSuppressionCBox->setFont( statusBarFont );
  //  mRenderSuppressionCBox->setToolTip( tr( "Toggle map rendering" ) );
  //  mStatusBar->addPermanentWidget( mRenderSuppressionCBox, 0 );


  // On the fly projection status bar icon
  // Changed this to a tool button since a QPushButton is
  // sculpted on OS X and the icon is never displayed [gsherman]
  mOnTheFlyProjectionStatusButton = new QToolButton( mStatusBar );
  mOnTheFlyProjectionStatusButton->setAutoRaise( true );
  mOnTheFlyProjectionStatusButton->setFont( statusBarFont );
  mOnTheFlyProjectionStatusButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
  mOnTheFlyProjectionStatusButton->setObjectName( QStringLiteral( "mOntheFlyProjectionStatusButton" ) );
  // Maintain uniform widget height in status bar by setting button height same as labels
  // For Qt/Mac 3.3, the default toolbutton height is 30 and labels were expanding to match
  mOnTheFlyProjectionStatusButton->setMaximumHeight( mScaleWidget->height() );
  mOnTheFlyProjectionStatusButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "mIconProjectionEnabled.svg" ) ) );
  mOnTheFlyProjectionStatusButton->setToolTip( tr( "CRS status - Click "
      "to open coordinate reference system dialog" ) );
  connect( mOnTheFlyProjectionStatusButton, &QAbstractButton::clicked,
           this, &QgisApp::projectPropertiesProjections );//bring up the project props dialog when clicked
  mStatusBar->addPermanentWidget( mOnTheFlyProjectionStatusButton, 0 );
  mStatusBar->showMessage( tr( "Ready" ) );

  mMessageButton = new QToolButton( mStatusBar );
  mMessageButton->setAutoRaise( true );
  mMessageButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mMessageLogRead.svg" ) ) );
  mMessageButton->setToolTip( tr( "Messages" ) );
  mMessageButton->setObjectName( QStringLiteral( "mMessageLogViewerButton" ) );
  mMessageButton->setMaximumHeight( mScaleWidget->height() );
  mMessageButton->setCheckable( true );
  mStatusBar->addPermanentWidget( mMessageButton, 0 );

  mLocatorWidget = new QgsLocatorWidget( mStatusBar );
  mStatusBar->addPermanentWidget( mLocatorWidget, 0, QgsStatusBar::AnchorLeft );
  QShortcut *locatorShortCut = new QShortcut( QKeySequence( tr( "Ctrl+K" ) ), this );
  connect( locatorShortCut, &QShortcut::activated, mLocatorWidget, [ = ] { mLocatorWidget->search( QString() ); } );
  locatorShortCut->setObjectName( QStringLiteral( "Locator" ) );
  locatorShortCut->setWhatsThis( tr( "Trigger Locator" ) );

  mLocatorWidget->locator()->registerFilter( new QgsLayerTreeLocatorFilter() );
  mLocatorWidget->locator()->registerFilter( new QgsLayoutLocatorFilter() );
  QList< QWidget *> actionObjects;
  actionObjects << menuBar()
                << mAdvancedDigitizeToolBar
                << mShapeDigitizeToolBar
                << mFileToolBar
                << mDataSourceManagerToolBar
                << mLayerToolBar
                << mDigitizeToolBar
                << mMapNavToolBar
                << mAttributesToolBar
                << mPluginToolBar
                << mRasterToolBar
                << mLabelToolBar
                << mVectorToolBar
                << mDatabaseToolBar
                << mWebToolBar
                << mSnappingToolBar;

  mLocatorWidget->locator()->registerFilter( new QgsActionLocatorFilter( actionObjects ) );
  mLocatorWidget->locator()->registerFilter( new QgsActiveLayerFeaturesLocatorFilter() );
  mLocatorWidget->locator()->registerFilter( new QgsAllLayersFeaturesLocatorFilter() );
  mLocatorWidget->locator()->registerFilter( new QgsExpressionCalculatorLocatorFilter() );
  mLocatorWidget->locator()->registerFilter( new QgsBookmarkLocatorFilter() );
  mLocatorWidget->locator()->registerFilter( new QgsSettingsLocatorFilter() );
  mLocatorWidget->locator()->registerFilter( new QgsGotoLocatorFilter() );

  mNominatimGeocoder = std::make_unique< QgsNominatimGeocoder>();
  mLocatorWidget->locator()->registerFilter( new QgsNominatimLocatorFilter( mNominatimGeocoder.get(), mMapCanvas ) );
}

void QgisApp::setIconSizes( int size )
{
  QSize iconSize = QSize( size, size );
  QSize panelIconSize = QgsGuiUtils::panelIconSize( iconSize );

  //Set the icon size of for all the toolbars created in the future.
  setIconSize( iconSize );

  //Change all current icon sizes.
  QList<QToolBar *> toolbars = findChildren<QToolBar *>();
  const auto constToolbars = toolbars;
  for ( QToolBar *toolbar : constToolbars )
  {
    QString className = toolbar->parent()->metaObject()->className();
    if ( className == QLatin1String( "QgisApp" ) )
    {
      toolbar->setIconSize( iconSize );
    }
    else
    {
      toolbar->setIconSize( panelIconSize );
    }
  }

  const auto constMLayoutDesignerDialogs = mLayoutDesignerDialogs;
  for ( QgsLayoutDesignerDialog *d : constMLayoutDesignerDialogs )
  {
    d->setIconSizes( size );
  }
}

void QgisApp::setTheme( const QString &themeName )
{
  /*
  Init the toolbar icons by setting the icon for each action.
  All toolbar/menu items must be a QAction in order for this
  to work.

  When new toolbar/menu QAction objects are added to the interface,
  add an entry below to set the icon

  PNG names must match those defined for the default theme. The
  default theme is installed in <prefix>/share/qgis/themes/default.

  New core themes can be added by creating a subdirectory under src/themes
  and modifying the appropriate CMakeLists.txt files. User contributed themes
  will be installed directly into <prefix>/share/qgis/themes/<themedir>.

  Themes can be selected from the preferences dialog. The dialog parses
  the themes directory and builds a list of themes (ie subdirectories)
  for the user to choose from.
  */

  QString theme = themeName;

  mStyleSheetBuilder->buildStyleSheet( mStyleSheetBuilder->defaultOptions() );
  QgsApplication::setUITheme( theme );

  mActionNewProject->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFileNew.svg" ) ) );
  mActionOpenProject->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFileOpen.svg" ) ) );
  mActionSaveProject->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFileSave.svg" ) ) );
  mActionSaveProjectAs->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFileSaveAs.svg" ) ) );
  mActionSaveMapAsImage->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSaveMapAsImage.svg" ) ) );
  mActionSaveMapAsPdf->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSaveAsPDF.svg" ) ) );
  mActionExit->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFileExit.png" ) ) );
  mActionAddOgrLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddOgrLayer.svg" ) ) );
  mActionAddRasterLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddRasterLayer.svg" ) ) );
#ifdef HAVE_POSTGRESQL
  mActionAddPgLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddPostgisLayer.svg" ) ) );
#endif
#ifdef HAVE_SPATIALITE
  mActionNewSpatiaLiteLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionNewSpatiaLiteLayer.svg" ) ) );
  mActionAddSpatiaLiteLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddSpatiaLiteLayer.svg" ) ) );
#endif
  mActionAddMssqlLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddMssqlLayer.svg" ) ) );
#ifdef HAVE_ORACLE
  mActionAddOracleLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddOracleLayer.svg" ) ) );
#endif
#ifdef HAVE_HANA
  mActionAddHanaLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddHanaLayer.svg" ) ) );
#endif
  mActionRemoveLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionRemoveLayer.svg" ) ) );
  mActionDuplicateLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDuplicateLayer.svg" ) ) );
  mActionSetLayerCRS->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSetLayerCRS.png" ) ) );
  mActionSetProjectCRSFromLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSetProjectCRSFromLayer.png" ) ) );
  mActionNewVectorLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionNewVectorLayer.svg" ) ) );
  mActionDataSourceManager->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDataSourceManager.svg" ) ) );
  mActionNewMemoryLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCreateMemory.svg" ) ) );
  mActionAddAllToOverview->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddAllToOverview.svg" ) ) );
  mActionHideAllLayers->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionHideAllLayers.svg" ) ) );
  mActionShowAllLayers->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowAllLayers.svg" ) ) );
  mActionHideSelectedLayers->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionHideSelectedLayers.svg" ) ) );
  mActionHideDeselectedLayers->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionHideDeselectedLayers.svg" ) ) );
  mActionShowSelectedLayers->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowSelectedLayers.svg" ) ) );
  mActionRemoveAllFromOverview->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionRemoveAllFromOverview.svg" ) ) );
  mActionToggleFullScreen->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionToggleFullScreen.png" ) ) );
  mActionProjectProperties->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionProjectProperties.svg" ) ) );
  mActionManagePlugins->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowPluginManager.svg" ) ) );
  mActionShowPythonDialog->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "console/mIconRunConsole.svg" ) ) );
  mActionCheckQgisVersion->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mIconSuccess.svg" ) ) );
  mActionOptions->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionOptions.svg" ) ) );
  mActionConfigureShortcuts->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionKeyboardShortcuts.svg" ) ) );
  mActionCustomization->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionInterfaceCustomization.svg" ) ) );
  mActionHelpContents->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionHelpContents.svg" ) ) );
  mActionLocalHistogramStretch->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionLocalHistogramStretch.svg" ) ) );
  mActionFullHistogramStretch->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFullHistogramStretch.svg" ) ) );
  mActionIncreaseBrightness->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionIncreaseBrightness.svg" ) ) );
  mActionDecreaseBrightness->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDecreaseBrightness.svg" ) ) );
  mActionIncreaseContrast->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionIncreaseContrast.svg" ) ) );
  mActionDecreaseContrast->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDecreaseContrast.svg" ) ) );
  mActionIncreaseGamma->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionIncreaseGamma.svg" ) ) );
  mActionDecreaseGamma->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDecreaseGamma.svg" ) ) );
  mActionZoomActualSize->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomNative.png" ) ) );
  mActionQgisHomePage->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionQgisHomePage.png" ) ) );
  mActionAbout->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionHelpAbout.svg" ) ) );
  mActionSponsors->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionHelpSponsors.png" ) ) );
  mActionDraw->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionRefresh.svg" ) ) );
  mActionToggleEditing->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionToggleEditing.svg" ) ) );
  mActionSaveLayerEdits->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSaveAllEdits.svg" ) ) );
  mActionAllEdits->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAllEdits.svg" ) ) );
  mActionSaveEdits->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSaveEdits.svg" ) ) );
  mActionSaveAllEdits->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSaveAllEdits.svg" ) ) );
  mActionRollbackEdits->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionRollbackEdits.svg" ) ) );
  mActionRollbackAllEdits->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionRollbackAllEdits.svg" ) ) );
  mActionCancelEdits->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCancelEdits.svg" ) ) );
  mActionCancelAllEdits->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCancelAllEdits.svg" ) ) );
  mActionCutFeatures->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionEditCut.svg" ) ) );
  mActionCopyFeatures->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionEditCopy.svg" ) ) );
  mActionPasteFeatures->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionEditPaste.svg" ) ) );
  mActionAddFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCapturePoint.svg" ) ) );
  mActionMoveFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveFeaturePoint.svg" ) ) );
  mActionMoveFeatureCopy->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveFeatureCopyPoint.svg" ) ) );
  mActionRotateFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionRotateFeature.svg" ) ) );
  mActionScaleFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionScaleFeature.svg" ) ) );
  mActionReshapeFeatures->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionReshape.svg" ) ) );
  mActionSplitFeatures->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSplitFeatures.svg" ) ) );
  mActionSplitParts->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSplitParts.svg" ) ) );
  mActionDeleteSelected->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDeleteSelectedFeatures.svg" ) ) );
  mActionVertexTool->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionVertexTool.svg" ) ) );
  mActionVertexToolActiveLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionVertexToolActiveLayer.svg" ) ) );
  mActionSimplifyFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSimplify.svg" ) ) );
  mActionUndo->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionUndo.svg" ) ) );
  mActionRedo->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionRedo.svg" ) ) );
  mActionAddRing->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddRing.svg" ) ) );
  mActionFillRing->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFillRing.svg" ) ) );
  mActionAddPart->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddPart.svg" ) ) );
  mActionDeleteRing->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDeleteRing.svg" ) ) );
  mActionDeletePart->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDeletePart.svg" ) ) );
  mActionMergeFeatures->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMergeFeatures.svg" ) ) );
  mActionOffsetCurve->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionOffsetCurve.svg" ) ) );
  mActionMergeFeatureAttributes->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMergeFeatureAttributes.svg" ) ) );
  mActionRotatePointSymbols->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "mActionRotatePointSymbols.svg" ) ) );
  mActionOffsetPointSymbol->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "mActionOffsetPointSymbols.svg" ) ) );
  mActionZoomIn->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomIn.svg" ) ) );
  mActionZoomOut->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomOut.svg" ) ) );
  mActionZoomFullExtent->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomFullExtent.svg" ) ) );
  mActionZoomToSelected->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomToSelected.svg" ) ) );
  mActionShowRasterCalculator->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowRasterCalculator.png" ) ) );
  mActionShowMeshCalculator->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowMeshCalculator.png" ) ) );
  mActionPan->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionPan.svg" ) ) );
  mActionPanToSelected->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionPanToSelected.svg" ) ) );
  mActionZoomLast->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomLast.svg" ) ) );
  mActionZoomNext->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomNext.svg" ) ) );
  mActionZoomToLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomToLayer.svg" ) ) );
  mActionZoomToLayers->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomToLayer.svg" ) ) );
  mActionZoomActualSize->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionZoomActual.svg" ) ) );
  mActionIdentify->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionIdentify.svg" ) ) );
  mActionFeatureAction->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mAction.svg" ) ) );
  mActionSelectFeatures->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSelectRectangle.svg" ) ) );
  mActionSelectPolygon->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSelectPolygon.svg" ) ) );
  mActionSelectFreehand->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSelectFreehand.svg" ) ) );
  mActionSelectRadius->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSelectRadius.svg" ) ) );
  mActionDeselectAll->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDeselectAll.svg" ) ) );
  mActionDeselectActiveLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionDeselectActiveLayer.svg" ) ) );
  mActionSelectAll->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSelectAll.svg" ) ) );
  mActionInvertSelection->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionInvertSelection.svg" ) ) );
  mActionSelectByExpression->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mIconExpressionSelect.svg" ) ) );
  mActionSelectByForm->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mIconFormSelect.svg" ) ) );
  mActionOpenTable->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionOpenTable.svg" ) ) );
  mActionOpenTableSelected->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionOpenTableSelected.svg" ) ) );
  mActionOpenTableVisible->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionOpenTableVisible.svg" ) ) );
  mActionOpenTableEdited->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionOpenTableEdited.svg" ) ) );
  mActionOpenFieldCalc->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCalculateField.svg" ) ) );
  mActionMeasure->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMeasure.svg" ) ) );
  mActionMeasureArea->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMeasureArea.svg" ) ) );
  mActionMeasureAngle->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMeasureAngle.svg" ) ) );
  mActionMeasureBearing->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMeasureBearing.svg" ) ) );
  mActionMapTips->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMapTips.svg" ) ) );
  mActionShowBookmarkManager->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowBookmarks.svg" ) ) );
  mActionShowBookmarks->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowBookmarks.svg" ) ) );
  mActionNewBookmark->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionNewBookmark.svg" ) ) );
  mActionCustomProjection->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCustomProjection.svg" ) ) );
  mActionAddWmsLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddWmsLayer.svg" ) ) );
  mActionAddXyzLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddXyzLayer.svg" ) ) );
  mActionAddVectorTileLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddVectorTileLayer.svg" ) ) );
  mActionAddWcsLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddWcsLayer.svg" ) ) );
#ifdef HAVE_SPATIALITE
  mActionAddWfsLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddWfsLayer.svg" ) ) );
#endif
  mActionAddAfsLayer->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddAfsLayer.svg" ) ) );
  mActionAddToOverview->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionInOverview.svg" ) ) );
  mActionAnnotation->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAnnotation.svg" ) ) );
  mActionFormAnnotation->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFormAnnotation.svg" ) ) );
  mActionHtmlAnnotation->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionHtmlAnnotation.svg" ) ) );
  mActionSvgAnnotation->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionSvgAnnotation.svg" ) ) );
  mActionTextAnnotation->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionTextAnnotation.svg" ) ) );
  mActionLabeling->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionLabeling.svg" ) ) );
  mActionShowPinnedLabels->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowPinnedLabels.svg" ) ) );
  mActionPinLabels->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionPinLabels.svg" ) ) );
  mActionShowHideLabels->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowHideLabels.svg" ) ) );
  mActionShowUnplacedLabels->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowUnplacedLabel.svg" ) ) );
  mActionMoveLabel->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveLabel.svg" ) ) );
  mActionRotateLabel->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionRotateLabel.svg" ) ) );
  mActionChangeLabelProperties->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionChangeLabelProperties.svg" ) ) );
  mActionDiagramProperties->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/propertyicons/diagram.svg" ) ) );
  mActionDecorationTitle->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/title_label.svg" ) ) );
  mActionDecorationCopyright->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/copyright_label.svg" ) ) );
  mActionDecorationImage->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddImage.svg" ) ) );
  mActionDecorationNorthArrow->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/north_arrow.svg" ) ) );
  mActionDecorationScaleBar->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionScaleBar.svg" ) ) );
  mActionDecorationGrid->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/grid.svg" ) ) );
  mActionReverseLine->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionReverseLine.svg" ) ) );
  mActionTrimExtendFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionTrimExtendFeature.svg" ) ) );
  mActionTemporalController->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/propertyicons/temporal.svg" ) ) );

  emit currentThemeChanged( themeName );
}

void QgisApp::setupConnections()
{
  // connect the "cleanup" slot
  connect( qApp, &QApplication::aboutToQuit, this, &QgisApp::saveWindowState );

  // signal when mouse moved over window (coords display in status bar)
  connect( mMapCanvas, &QgsMapCanvas::xyCoordinates, this, &QgisApp::saveLastMousePosition );
  connect( mMapCanvas, &QgsMapCanvas::extentsChanged, this, &QgisApp::extentChanged );
  connect( mMapCanvas, &QgsMapCanvas::scaleChanged, this, &QgisApp::showScale );
  connect( mMapCanvas, &QgsMapCanvas::rotationChanged, this, &QgisApp::showRotation );
  connect( mMapCanvas, &QgsMapCanvas::scaleChanged, this, &QgisApp::updateMouseCoordinatePrecision );
  connect( mMapCanvas, &QgsMapCanvas::mapToolSet, this, &QgisApp::mapToolChanged );
  connect( mMapCanvas, &QgsMapCanvas::selectionChanged, this, &QgisApp::selectionChanged );
  connect( mMapCanvas, &QgsMapCanvas::layersChanged, this, &QgisApp::markDirty );

  connect( mMapCanvas, &QgsMapCanvas::zoomLastStatusChanged, mActionZoomLast, &QAction::setEnabled );
  connect( mMapCanvas, &QgsMapCanvas::zoomNextStatusChanged, mActionZoomNext, &QAction::setEnabled );

  // SG change
  //  connect( mRenderSuppressionCBox, &QAbstractButton::toggled, this, [ = ]( bool flag )
  //  {
  //    const auto canvases = mapCanvases();
  //    for ( QgsMapCanvas *canvas : canvases )
  //      canvas->setRenderFlag( flag );
  //    if ( !flag )
  //      canvasRefreshFinished(); // deals with the busy indicator in case of ongoing rendering
  //  } );

  connect( mMapCanvas, &QgsMapCanvas::destinationCrsChanged, this, &QgisApp::reprojectAnnotations );

  // connect MapCanvas keyPress event so we can check if selected feature collection must be deleted
  connect( mMapCanvas, &QgsMapCanvas::keyPressed, this, &QgisApp::mapCanvas_keyPressed );

  // project crs connections
  connect( QgsProject::instance(), &QgsProject::crsChanged, this, &QgisApp::projectCrsChanged );

  connect( QgsProject::instance()->viewSettings(), &QgsProjectViewSettings::mapScalesChanged, this, [ = ] { mScaleWidget->updateScales(); } );

  connect( QgsProject::instance(), &QgsProject::missingDatumTransforms, this, [ = ]( const QStringList & transforms )
  {
    QString message = tr( "Transforms are not installed: %1 " ).arg( transforms.join( QLatin1String( " ," ) ) );
    messageBar()->pushWarning( tr( "Missing datum transforms" ), message );
  } );

  connect( QgsProject::instance(), &QgsProject::labelingEngineSettingsChanged,
           mMapCanvas, [ = ]
  {
    mMapCanvas->setLabelingEngineSettings( QgsProject::instance()->labelingEngineSettings() );
  } );

  connect( QgsProject::instance(), &QgsProject::backgroundColorChanged, this, [ = ]
  {
    const QColor backgroundColor = QgsProject::instance()->backgroundColor();
    const auto constMapCanvases = mapCanvases();
    for ( QgsMapCanvas *canvas : constMapCanvases )
    {
      canvas->setCanvasColor( backgroundColor );
    }
    if ( auto *lMapOverviewCanvas = mapOverviewCanvas() )
    {
      lMapOverviewCanvas->setBackgroundColor( backgroundColor );
      lMapOverviewCanvas->refresh();
    }
  } );

  connect( QgsProject::instance(), &QgsProject::selectionColorChanged, this, [ = ]
  {
    const QColor selectionColor = QgsProject::instance()->selectionColor();
    const auto constMapCanvases = mapCanvases();
    for ( QgsMapCanvas *canvas : constMapCanvases )
    {
      canvas->setSelectionColor( selectionColor );
    }
  } );

  connect( QgsProject::instance()->timeSettings(), &QgsProjectTimeSettings::temporalRangeChanged, this, &QgisApp::projectTemporalRangeChanged );

  // connect legend signals
  connect( this, &QgisApp::activeLayerChanged,
           this, &QgisApp::activateDeactivateLayerRelatedActions );
  connect( this, &QgisApp::activeLayerChanged,
           this, &QgisApp::setMapStyleDockLayer );
  connect( mLayerTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
           this, &QgisApp::legendLayerSelectionChanged );
  connect( mLayerTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
           this, &QgisApp::activateDeactivateMultipleLayersRelatedActions );
  connect( mLayerTreeView->layerTreeModel()->rootGroup(), &QgsLayerTreeNode::addedChildren,
           this, &QgisApp::markDirty );
  connect( mLayerTreeView->layerTreeModel()->rootGroup(), &QgsLayerTreeNode::addedChildren,
           this, &QgisApp::updateNewLayerInsertionPoint );
  connect( mLayerTreeView->layerTreeModel()->rootGroup(), &QgsLayerTreeNode::removedChildren,
           this, &QgisApp::markDirty );
  connect( mLayerTreeView->layerTreeModel()->rootGroup(), &QgsLayerTreeNode::removedChildren,
           this, &QgisApp::updateNewLayerInsertionPoint );
  connect( mLayerTreeView->layerTreeModel()->rootGroup(), &QgsLayerTreeNode::visibilityChanged,
           this, &QgisApp::markDirty );
  connect( mLayerTreeView->layerTreeModel()->rootGroup(), &QgsLayerTreeNode::customPropertyChanged,
           this, [ = ]( QgsLayerTreeNode *, const QString & key )
  {
    // only mark dirty for non-view only changes
    if ( !QgsLayerTreeView::viewOnlyCustomProperties().contains( key ) )
      QgisApp::markDirty();
  } );

  // connect map layer registry
  connect( QgsProject::instance(), &QgsProject::layersAdded,
           this, &QgisApp::layersWereAdded );
  connect( QgsProject::instance(),
           static_cast < void ( QgsProject::* )( const QStringList & ) >( &QgsProject::layersWillBeRemoved ),
           this, &QgisApp::removingLayers );

  // connect initialization signal
  connect( this, &QgisApp::initializationCompleted,
           this, &QgisApp::fileOpenAfterLaunch );

  // Connect warning dialog from project reading
  connect( QgsProject::instance(), &QgsProject::readVersionMismatchOccurred,
           this, &QgisApp::projectVersionMismatchOccurred );
  connect( QgsProject::instance(), &QgsProject::layerLoaded,
           this, [this]( int i, int n )
  {
    if ( !mProjectLoadingProxyTask && i < n )
    {
      const QString name = QgsProject::instance()->title().isEmpty() ? QgsProject::instance()->fileName() : QgsProject::instance()->title();
      mProjectLoadingProxyTask = new QgsProxyProgressTask( tr( "Loading “%1”" ).arg( name ) );
      QgsApplication::taskManager()->addTask( mProjectLoadingProxyTask );
    }

    if ( mProjectLoadingProxyTask )
    {
      mProjectLoadingProxyTask->setProxyProgress( 100.0 * static_cast< double >( i ) / n );
      if ( i >= n )
      {
        mProjectLoadingProxyTask->finalize( true );
        mProjectLoadingProxyTask = nullptr;
      }
    }
  } );
  connect( QgsProject::instance(), &QgsProject::loadingLayer,
           this, &QgisApp::showStatusMessage );
  connect( QgsProject::instance(), &QgsProject::loadingLayerMessageReceived,
           this, &QgisApp::loadingLayerMessages );
  connect( QgsProject::instance(), &QgsProject::readProject,
           this, &QgisApp::readProject );
  connect( QgsProject::instance(), &QgsProject::writeProject,
           this, &QgisApp::writeProject );

  connect( this, &QgisApp::projectRead,
           this, &QgisApp::fileOpenedOKAfterLaunch );

  connect( QgsProject::instance(), &QgsProject::transactionGroupsChanged, this, &QgisApp::onTransactionGroupsChanged );

  // connect preview modes actions
  connect( mActionPreviewModeOff, &QAction::triggered, this, &QgisApp::disablePreviewMode );
  connect( mActionPreviewModeMono, &QAction::triggered, this, &QgisApp::activateMonoPreview );
  connect( mActionPreviewModeGrayscale, &QAction::triggered, this, &QgisApp::activateGrayscalePreview );
  connect( mActionPreviewProtanope, &QAction::triggered, this, &QgisApp::activateProtanopePreview );
  connect( mActionPreviewDeuteranope, &QAction::triggered, this, &QgisApp::activateDeuteranopePreview );
  connect( mActionPreviewTritanope, &QAction::triggered, this, &QgisApp::activateTritanopePreview );

  // setup undo/redo actions
  connect( mUndoWidget, &QgsUndoWidget::undoStackChanged, this, &QgisApp::updateUndoActions );

  connect( mLayoutsMenu, &QMenu::aboutToShow, this, &QgisApp::layoutsMenuAboutToShow );

  connect( m3DMapViewsMenu, &QMenu::aboutToShow, this, &QgisApp::views3DMenuAboutToShow );
}

void QgisApp::setupCanvasTools()
{
  mMapTools->mapTool( QgsAppMapTools::ZoomIn )->setAction( mActionZoomIn );
  mMapTools->mapTool( QgsAppMapTools::ZoomOut )->setAction( mActionZoomOut );
  connect( mMapTools->mapTool< QgsMapToolPan >( QgsAppMapTools::Pan ), &QgsMapToolPan::panDistanceBearingChanged, this, &QgisApp::showPanMessage );
  mMapTools->mapTool( QgsAppMapTools::Pan )->setAction( mActionPan );
  mMapTools->mapTool( QgsAppMapTools::Identify )->setAction( mActionIdentify );
  connect( mMapTools->mapTool< QgsMapToolIdentifyAction >( QgsAppMapTools::Identify ), &QgsMapToolIdentifyAction::copyToClipboard,
           this, &QgisApp::copyFeatures );
  mMapTools->mapTool( QgsAppMapTools::FeatureAction )->setAction( mActionFeatureAction );
  mMapTools->mapTool( QgsAppMapTools::MeasureDistance )->setAction( mActionMeasure );
  mMapTools->mapTool( QgsAppMapTools::MeasureArea )->setAction( mActionMeasureArea );
  mMapTools->mapTool( QgsAppMapTools::MeasureAngle )->setAction( mActionMeasureAngle );
  mMapTools->mapTool( QgsAppMapTools::MeasureBearing )->setAction( mActionMeasureBearing );
  mMapTools->mapTool( QgsAppMapTools::TextAnnotation )->setAction( mActionTextAnnotation );
  mMapTools->mapTool( QgsAppMapTools::FormAnnotation )->setAction( mActionFormAnnotation );
  mMapTools->mapTool( QgsAppMapTools::HtmlAnnotation )->setAction( mActionHtmlAnnotation );
  mMapTools->mapTool( QgsAppMapTools::SvgAnnotation )->setAction( mActionSvgAnnotation );
  mMapTools->mapTool( QgsAppMapTools::Annotation )->setAction( mActionAnnotation );
  mMapTools->mapTool( QgsAppMapTools::AddFeature )->setAction( mActionAddFeature );
  mMapTools->mapTool( QgsAppMapTools::MoveFeature )->setAction( mActionMoveFeature );
  mMapTools->mapTool( QgsAppMapTools::MoveFeatureCopy )->setAction( mActionMoveFeatureCopy );
  mMapTools->mapTool( QgsAppMapTools::RotateFeature )->setAction( mActionRotateFeature );
  mMapTools->mapTool( QgsAppMapTools::ScaleFeature )->setAction( mActionScaleFeature );
  mMapTools->mapTool( QgsAppMapTools::OffsetCurve )->setAction( mActionOffsetCurve );
  mMapTools->mapTool( QgsAppMapTools::ReshapeFeatures )->setAction( mActionReshapeFeatures );
  mMapTools->mapTool( QgsAppMapTools::ReverseLine )->setAction( mActionReverseLine );
  mMapTools->mapTool( QgsAppMapTools::SplitFeatures )->setAction( mActionSplitFeatures );
  mMapTools->mapTool( QgsAppMapTools::SplitParts )->setAction( mActionSplitParts );
  mMapTools->mapTool( QgsAppMapTools::SelectFeatures )->setAction( mActionSelectFeatures );
  mMapTools->mapTool<QgsMapToolSelect>( QgsAppMapTools::SelectFeatures )->setSelectionMode( QgsMapToolSelectionHandler::SelectSimple );
  connect( mMapTools->mapTool<QgsMapToolSelect>( QgsAppMapTools::SelectFeatures ), &QgsMapToolSelect::modeChanged, this, &QgisApp::selectionModeChanged );
  mMapTools->mapTool( QgsAppMapTools::SelectPolygon )->setAction( mActionSelectPolygon );
  mMapTools->mapTool<QgsMapToolSelect>( QgsAppMapTools::SelectPolygon )->setSelectionMode( QgsMapToolSelectionHandler::SelectPolygon );
  connect( mMapTools->mapTool<QgsMapToolSelect>( QgsAppMapTools::SelectPolygon ), &QgsMapToolSelect::modeChanged, this, &QgisApp::selectionModeChanged );
  mMapTools->mapTool( QgsAppMapTools::SelectFreehand )->setAction( mActionSelectFreehand );
  mMapTools->mapTool<QgsMapToolSelect>( QgsAppMapTools::SelectFreehand )->setSelectionMode( QgsMapToolSelectionHandler::SelectFreehand );
  connect( mMapTools->mapTool<QgsMapToolSelect>( QgsAppMapTools::SelectFreehand ), &QgsMapToolSelect::modeChanged, this, &QgisApp::selectionModeChanged );
  mMapTools->mapTool( QgsAppMapTools::SelectRadius )->setAction( mActionSelectRadius );
  mMapTools->mapTool<QgsMapToolSelect>( QgsAppMapTools::SelectRadius )->setSelectionMode( QgsMapToolSelectionHandler::SelectRadius );
  connect( mMapTools->mapTool<QgsMapToolSelect>( QgsAppMapTools::SelectRadius ), &QgsMapToolSelect::modeChanged, this, &QgisApp::selectionModeChanged );
  mMapTools->mapTool( QgsAppMapTools::AddRing )->setAction( mActionAddRing );
  mMapTools->mapTool( QgsAppMapTools::FillRing )->setAction( mActionFillRing );
  mMapTools->mapTool( QgsAppMapTools::AddPart )->setAction( mActionAddPart );
  mMapTools->mapTool( QgsAppMapTools::SimplifyFeature )->setAction( mActionSimplifyFeature );
  mMapTools->mapTool( QgsAppMapTools::DeleteRing )->setAction( mActionDeleteRing );
  mMapTools->mapTool( QgsAppMapTools::DeletePart )->setAction( mActionDeletePart );
  mMapTools->mapTool( QgsAppMapTools::VertexTool )->setAction( mActionVertexTool );
  mMapTools->mapTool( QgsAppMapTools::VertexToolActiveLayer )->setAction( mActionVertexToolActiveLayer );
  mMapTools->mapTool( QgsAppMapTools::RotatePointSymbolsTool )->setAction( mActionRotatePointSymbols );
  mMapTools->mapTool( QgsAppMapTools::OffsetPointSymbolTool )->setAction( mActionOffsetPointSymbol );
  mMapTools->mapTool( QgsAppMapTools::TrimExtendFeature )->setAction( mActionTrimExtendFeature );
  mMapTools->mapTool( QgsAppMapTools::PinLabels )->setAction( mActionPinLabels );
  mMapTools->mapTool( QgsAppMapTools::ShowHideLabels )->setAction( mActionShowHideLabels );
  mMapTools->mapTool( QgsAppMapTools::MoveLabel )->setAction( mActionMoveLabel );
  mMapTools->mapTool( QgsAppMapTools::RotateLabel )->setAction( mActionRotateLabel );
  mMapTools->mapTool( QgsAppMapTools::ChangeLabelProperties )->setAction( mActionChangeLabelProperties );
  mMapTools->mapTool( QgsAppMapTools::AnnotationEdit )->setAction( mActionModifyAnnotation );

  //ensure that non edit tool is initialized or we will get crashes in some situations
  mNonEditMapTool = mMapTools->mapTool( QgsAppMapTools::Pan );

  mDigitizingTechniqueManager->setupCanvasTools();
}

void QgisApp::createOverview()
{
  // overview canvas
  mOverviewCanvas = new QgsMapOverviewCanvas( nullptr, mMapCanvas );

  //set canvas color to default
  QgsSettings settings;
  int red = settings.value( QStringLiteral( "qgis/default_canvas_color_red" ), 255 ).toInt();
  int green = settings.value( QStringLiteral( "qgis/default_canvas_color_green" ), 255 ).toInt();
  int blue = settings.value( QStringLiteral( "qgis/default_canvas_color_blue" ), 255 ).toInt();
  mOverviewCanvas->setBackgroundColor( QColor( red, green, blue ) );

  mOverviewMapCursor = new QCursor( Qt::OpenHandCursor );
  mOverviewCanvas->setCursor( *mOverviewMapCursor );
//  QVBoxLayout *myOverviewLayout = new QVBoxLayout;
//  myOverviewLayout->addWidget(overviewCanvas);
//  overviewFrame->setLayout(myOverviewLayout);
  mOverviewDock = new QgsDockWidget( tr( "Overview" ), this );

  QShortcut *showOverviewDock = new QShortcut( QKeySequence( tr( "Ctrl+8" ) ), this );
  connect( showOverviewDock, &QShortcut::activated, mOverviewDock, &QgsDockWidget::toggleUserVisible );
  showOverviewDock->setObjectName( QStringLiteral( "ShowOverviewPanel" ) );
  showOverviewDock->setWhatsThis( tr( "Show Overview Panel" ) );

  mOverviewDock->setObjectName( QStringLiteral( "Overview" ) );
  mOverviewDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  mOverviewDock->setWidget( mOverviewCanvas );
  addDockWidget( Qt::LeftDockWidgetArea, mOverviewDock );
  // add to the Panel submenu
  mPanelMenu->addAction( mOverviewDock->toggleViewAction() );

  mLayerTreeCanvasBridge->setOverviewCanvas( mOverviewCanvas );
}

void QgisApp::addDockWidget( Qt::DockWidgetArea area, QDockWidget *thepDockWidget )
{
  QMainWindow::addDockWidget( area, thepDockWidget );
  // Make the right and left docks consume all vertical space and top
  // and bottom docks nest between them
  setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
  setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
  setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
  setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
  // add to the Panel submenu
  mPanelMenu->addAction( thepDockWidget->toggleViewAction() );

  thepDockWidget->show();

  // refresh the map canvas
  refreshMapCanvas();
}

void QgisApp::removeDockWidget( QDockWidget *thepDockWidget )
{
  QMainWindow::removeDockWidget( thepDockWidget );
  mPanelMenu->removeAction( thepDockWidget->toggleViewAction() );
}

QToolBar *QgisApp::addToolBar( const QString &name )
{
  QToolBar *toolBar = QMainWindow::addToolBar( name );
  // add to the Toolbar submenu
  mToolbarMenu->addAction( toolBar->toggleViewAction() );
  return toolBar;
}

void QgisApp::addToolBar( QToolBar *toolBar, Qt::ToolBarArea area )
{
  QMainWindow::addToolBar( area, toolBar );
  // add to the Toolbar submenu
  mToolbarMenu->addAction( toolBar->toggleViewAction() );
}

QgsLayerTreeView *QgisApp::layerTreeView()
{
  Q_ASSERT( mLayerTreeView );
  return mLayerTreeView;
}

QgsPluginManager *QgisApp::pluginManager()
{
  Q_ASSERT( mPluginManager );
  return mPluginManager;
}

QgsUserProfileManager *QgisApp::userProfileManager()
{
  Q_ASSERT( mUserProfileManager );
  return mUserProfileManager;
}

QgsMapCanvas *QgisApp::mapCanvas()
{
  Q_ASSERT( mMapCanvas );
  return mMapCanvas;
}

QgsMapCanvas *QgisApp::createNewMapCanvas( const QString &name )
{
  QgsMapCanvasDockWidget *dock = createNewMapCanvasDock( name );
  if ( !dock )
    return nullptr;

  setupDockWidget( dock );  // use default dock position settings

  dock->mapCanvas()->setLayers( mMapCanvas->layers() );
  dock->mapCanvas()->setExtent( mMapCanvas->extent() );
  QgsDebugMsgLevel( QStringLiteral( "QgisApp::createNewMapCanvas -2- : QgsProject::instance()->crs().description[%1]ellipsoid[%2]" ).arg( QgsProject::instance()->crs().description(), QgsProject::instance()->crs().ellipsoidAcronym() ), 3 );
  dock->mapCanvas()->setDestinationCrs( QgsProject::instance()->crs() );
  dock->mapCanvas()->freeze( false );
  return dock->mapCanvas();
}

QgsMapCanvasDockWidget *QgisApp::createNewMapCanvasDock( const QString &name )
{
  const auto canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
  {
    if ( canvas->objectName() == name )
    {
      QgsDebugMsg( QStringLiteral( "A map canvas with name '%1' already exists!" ).arg( name ) );
      return nullptr;
    }
  }

  QgsMapCanvasDockWidget *mapCanvasWidget = new QgsMapCanvasDockWidget( name, this );
  mapCanvasWidget->setAllowedAreas( Qt::AllDockWidgetAreas );
  mapCanvasWidget->setMainCanvas( mMapCanvas );

  QgsMapCanvas *mapCanvas = mapCanvasWidget->mapCanvas();
  mapCanvas->freeze( true );
  mapCanvas->setObjectName( name );
  mapCanvas->setProject( QgsProject::instance() );
  connect( mapCanvas, &QgsMapCanvas::messageEmitted, this, &QgisApp::displayMessage );
  connect( mLayerTreeCanvasBridge, &QgsLayerTreeMapCanvasBridge::canvasLayersChanged, mapCanvas, &QgsMapCanvas::setLayers );

  applyProjectSettingsToCanvas( mapCanvas );
  applyDefaultSettingsToCanvas( mapCanvas );

  // add existing annotations to canvas
  const auto constAnnotations = QgsProject::instance()->annotationManager()->annotations();
  for ( QgsAnnotation *annotation : constAnnotations )
  {
    QgsMapCanvasAnnotationItem *canvasItem = new QgsMapCanvasAnnotationItem( annotation, mapCanvas );
    Q_UNUSED( canvasItem ) //item is already added automatically to canvas scene
  }

  mapCanvas->setCustomDropHandlers( mCustomDropHandlers );

  markDirty();
  connect( mapCanvasWidget, &QgsMapCanvasDockWidget::closed, this, &QgisApp::markDirty );
  connect( mapCanvasWidget, &QgsMapCanvasDockWidget::renameTriggered, this, &QgisApp::renameView );

  return mapCanvasWidget;
}


void QgisApp::setupDockWidget( QDockWidget *dockWidget, bool isFloating, QRect dockGeometry, Qt::DockWidgetArea area )
{
  dockWidget->setFloating( isFloating );
  if ( dockGeometry.isEmpty() )
  {
    // try to guess a nice initial placement for view - about 3/4 along, half way down
    dockWidget->setGeometry( QRect( static_cast< int >( rect().width() * 0.75 ), static_cast< int >( rect().height() * 0.5 ), 400, 400 ) );
    addDockWidget( area, dockWidget );
  }
  else
  {
    if ( !isFloating )
    {
      // ugly hack, but only way to set dock size correctly for Qt < 5.6
      dockWidget->setFixedSize( dockGeometry.size() );
      addDockWidget( area, dockWidget );
      dockWidget->resize( dockGeometry.size() );
      QgsApplication::processEvents(); // required!
      dockWidget->setFixedSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX );
    }
    else
    {
      dockWidget->setGeometry( dockGeometry );
      addDockWidget( area, dockWidget );
    }
  }
}

void QgisApp::closeMapCanvas( const QString &name )
{
  const auto dockWidgets = findChildren< QgsMapCanvasDockWidget * >();
  for ( QgsMapCanvasDockWidget *w : dockWidgets )
  {
    if ( w->mapCanvas()->objectName() == name )
    {
      w->close();
      delete w;
      break;
    }
  }
}

void QgisApp::closeAdditionalMapCanvases()
{
  QgsCanvasRefreshBlocker refreshBlocker; // closing docks may cause canvases to resize, and we don't want a map refresh occurring
  const auto dockWidgets = findChildren< QgsMapCanvasDockWidget * >();
  for ( QgsMapCanvasDockWidget *w : dockWidgets )
  {
    w->close();
    delete w;
  }
}

void QgisApp::closeAdditional3DMapCanvases()
{
#ifdef HAVE_3D
  QSet<Qgs3DMapCanvasWidget *> openDocks = mOpen3DMapViews;
  for ( Qgs3DMapCanvasWidget *w : openDocks )
  {
    close3DMapView( w->canvasName() );
  }
#endif
}

void QgisApp::freezeCanvases( bool frozen )
{
  const auto canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
  {
    canvas->freeze( frozen );
  }
}

QgsMessageBar *QgisApp::messageBar()
{
  // Q_ASSERT( mInfoBar );
  return mInfoBar;
}

void QgisApp::toggleLogMessageIcon( bool hasLogMessage )
{
  if ( hasLogMessage && !mLogDock->isVisible() )
  {
    mMessageButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mMessageLog.svg" ) ) );
  }
  else
  {
    mMessageButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mMessageLogRead.svg" ) ) );
  }
}

void QgisApp::openMessageLog()
{
  mLogDock->setUserVisible( true );
}

void QgisApp::addUserInputWidget( QWidget *widget )
{
  mUserInputDockWidget->addUserInputWidget( widget );
}

void QgisApp::initLayerTreeView()
{

    // SG add to not show the empty layer dock on start
    //  mLayerTreeDock = new QgsDockWidget( tr( "Layers" ), this );
    //  mLayerTreeDock->setObjectName( QStringLiteral( "Layers" ) );
    //  mLayerTreeDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

    //  QShortcut *showLayersTreeDock = new QShortcut( QKeySequence( tr( "Ctrl+1" ) ), this );
    //  connect( showLayersTreeDock, &QShortcut::activated, mLayerTreeDock, &QgsDockWidget::toggleUserVisible );
    //  showLayersTreeDock->setObjectName( QStringLiteral( "ShowLayersPanel" ) );
    //  showLayersTreeDock->setWhatsThis( tr( "Show Layers Panel" ) );

  QgsLayerTreeModel *model = new QgsLayerTreeModel( QgsProject::instance()->layerTreeRoot(), this );
#ifdef ENABLE_MODELTEST
  new ModelTest( model, this );
#endif
  model->setFlag( QgsLayerTreeModel::AllowNodeReorder );
  model->setFlag( QgsLayerTreeModel::AllowNodeRename );
  model->setFlag( QgsLayerTreeModel::AllowNodeChangeVisibility );
  model->setFlag( QgsLayerTreeModel::ShowLegendAsTree );
  model->setFlag( QgsLayerTreeModel::UseEmbeddedWidgets );
  model->setFlag( QgsLayerTreeModel::UseTextFormatting );
  model->setAutoCollapseLegendNodes( 10 );

  mLayerTreeView->setModel( model );
  mLayerTreeView->setMessageBar( mInfoBar );

  mLayerTreeView->setMenuProvider( new QgsAppLayerTreeViewMenuProvider( mLayerTreeView, mMapCanvas ) );
  new QgsLayerTreeViewFilterIndicatorProvider( mLayerTreeView );  // gets parented to the layer view
  new QgsLayerTreeViewEmbeddedIndicatorProvider( mLayerTreeView );  // gets parented to the layer view
  new QgsLayerTreeViewMemoryIndicatorProvider( mLayerTreeView );  // gets parented to the layer view
  new QgsLayerTreeViewNotesIndicatorProvider( mLayerTreeView );  // gets parented to the layer view
  new QgsLayerTreeViewTemporalIndicatorProvider( mLayerTreeView ); // gets parented to the layer view
  new QgsLayerTreeViewNoCrsIndicatorProvider( mLayerTreeView );  // gets parented to the layer view
  new QgsLayerTreeViewOfflineIndicatorProvider( mLayerTreeView );  // gets parented to the layer view
  new QgsLayerTreeViewLowAccuracyIndicatorProvider( mLayerTreeView );  // gets parented to the layer view
  QgsLayerTreeViewBadLayerIndicatorProvider *badLayerIndicatorProvider = new QgsLayerTreeViewBadLayerIndicatorProvider( mLayerTreeView );  // gets parented to the layer view
  connect( badLayerIndicatorProvider, &QgsLayerTreeViewBadLayerIndicatorProvider::requestChangeDataSource, this, &QgisApp::changeDataSource );
  new QgsLayerTreeViewNonRemovableIndicatorProvider( mLayerTreeView );  // gets parented to the layer view

  setupLayerTreeViewFromSettings();

  connect( mLayerTreeView, &QAbstractItemView::doubleClicked, this, &QgisApp::layerTreeViewDoubleClicked );
  connect( mLayerTreeView, &QgsLayerTreeView::currentLayerChanged, this, &QgisApp::onActiveLayerChanged );
  connect( mLayerTreeView->selectionModel(), &QItemSelectionModel::currentChanged, this, &QgisApp::updateNewLayerInsertionPoint );
  connect( QgsProject::instance()->layerTreeRegistryBridge(), &QgsLayerTreeRegistryBridge::addedLayersToLayerTree,
           this, &QgisApp::autoSelectAddedLayer );

  // add group action
  QAction *actionAddGroup = new QAction( tr( "Add Group" ), this );
  actionAddGroup->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionAddGroup.svg" ) ) );
  actionAddGroup->setToolTip( tr( "Add Group" ) );
  connect( actionAddGroup, &QAction::triggered, mLayerTreeView->defaultActions(), &QgsLayerTreeViewDefaultActions::addGroup );

  // visibility groups tool button
  QToolButton *btnVisibilityPresets = new QToolButton;
  btnVisibilityPresets->setAutoRaise( true );
  btnVisibilityPresets->setToolTip( tr( "Manage Map Themes" ) );
  btnVisibilityPresets->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionShowAllLayers.svg" ) ) );
  btnVisibilityPresets->setPopupMode( QToolButton::InstantPopup );
  btnVisibilityPresets->setMenu( QgsMapThemes::instance()->menu() );

  // filter legend actions
  mFilterLegendToolButton = new QToolButton( this );
  mFilterLegendToolButton->setAutoRaise( true );
  mFilterLegendToolButton->setToolTip( tr( "Filter Legend" ) );
  mFilterLegendToolButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionFilter2.svg" ) ) );
  mFilterLegendToolButton->setPopupMode( QToolButton::InstantPopup );
  QMenu *filterLegendMenu = new QMenu( this );
  mFilterLegendToolButton->setMenu( filterLegendMenu );
  mFilterLegendByMapContentAction = new QAction( tr( "Filter Legend by Map Content" ), this );
  mFilterLegendByMapContentAction->setCheckable( true );
  connect( mFilterLegendByMapContentAction, &QAction::toggled, this, &QgisApp::updateFilterLegend );
  filterLegendMenu->addAction( mFilterLegendByMapContentAction );

  mFilterLegendToggleShowPrivateLayersAction = new QAction( tr( "Show Private Layers" ), this );
  mFilterLegendToggleShowPrivateLayersAction->setCheckable( true );
  connect( mFilterLegendToggleShowPrivateLayersAction, &QAction::toggled, this, [ = ]( bool showPrivateLayers ) { layerTreeView()->setShowPrivateLayers( showPrivateLayers ); } );
  filterLegendMenu->addAction( mFilterLegendToggleShowPrivateLayersAction );

  mLegendExpressionFilterButton = new QgsLegendFilterButton( this );
  mLegendExpressionFilterButton->setToolTip( tr( "Filter legend by expression" ) );
  connect( mLegendExpressionFilterButton, &QAbstractButton::toggled, this, &QgisApp::toggleFilterLegendByExpression );

  mActionStyleDock = new QAction( tr( "Layer Styling" ), this );
  mActionStyleDock->setCheckable( true );
  mActionStyleDock->setToolTip( tr( "Open the Layer Styling panel" ) );
  mActionStyleDock->setShortcut( QStringLiteral( "F7" ) );
  mActionStyleDock->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "propertyicons/symbology.svg" ) ) );
  connect( mActionStyleDock, &QAction::toggled, this, &QgisApp::mapStyleDock );

  // expand / collapse tool buttons
  QAction *actionExpandAll = new QAction( tr( "Expand All" ), this );
  actionExpandAll->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionExpandTree.svg" ) ) );
  actionExpandAll->setToolTip( tr( "Expand All" ) );
  connect( actionExpandAll, &QAction::triggered, mLayerTreeView, &QgsLayerTreeView::expandAllNodes );
  QAction *actionCollapseAll = new QAction( tr( "Collapse All" ), this );
  actionCollapseAll->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCollapseTree.svg" ) ) );
  actionCollapseAll->setToolTip( tr( "Collapse All" ) );
  connect( actionCollapseAll, &QAction::triggered, mLayerTreeView, &QgsLayerTreeView::collapseAllNodes );

  QToolBar *toolbar = new QToolBar();
  toolbar->setIconSize( iconSize( true ) );
  toolbar->addAction( mActionStyleDock );
  toolbar->addAction( actionAddGroup );
  toolbar->addWidget( btnVisibilityPresets );
  toolbar->addWidget( mFilterLegendToolButton );
  toolbar->addWidget( mLegendExpressionFilterButton );
  toolbar->addAction( actionExpandAll );
  toolbar->addAction( actionCollapseAll );
  toolbar->addAction( mActionRemoveLayer );

  QVBoxLayout *vboxLayout = new QVBoxLayout;
  vboxLayout->setContentsMargins( 0, 0, 0, 0 );
  vboxLayout->setSpacing( 0 );
  vboxLayout->addWidget( toolbar );
  vboxLayout->addWidget( mLayerTreeView );

  QWidget *w = new QWidget;
  w->setLayout( vboxLayout );

  // SG change
  //  mLayerTreeDock->setWidget( w );
  //  addDockWidget( Qt::LeftDockWidgetArea, mLayerTreeDock );

  mLayerTreeCanvasBridge = new QgsLayerTreeMapCanvasBridge( QgsProject::instance()->layerTreeRoot(), mMapCanvas, this );

  mMapLayerOrder = new QgsCustomLayerOrderWidget( mLayerTreeCanvasBridge, this );
  mMapLayerOrder->setObjectName( QStringLiteral( "theMapLayerOrder" ) );

  mLayerOrderDock = new QgsDockWidget( tr( "Layer Order" ), this );
  mLayerOrderDock->setObjectName( QStringLiteral( "LayerOrder" ) );
  mLayerOrderDock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

  QShortcut *showLayerOrderDock = new QShortcut( QKeySequence( tr( "Ctrl+9" ) ), this );
  connect( showLayerOrderDock, &QShortcut::activated, mLayerOrderDock, &QgsDockWidget::toggleUserVisible );
  showLayerOrderDock->setObjectName( QStringLiteral( "ShowLayerOrderPanel" ) );
  showLayerOrderDock->setWhatsThis( tr( "Show Layer Order Panel" ) );

  mLayerOrderDock->setWidget( mMapLayerOrder );
  addDockWidget( Qt::LeftDockWidgetArea, mLayerOrderDock );
  mLayerOrderDock->hide();

  connect( mMapCanvas, &QgsMapCanvas::mapCanvasRefreshed, this, &QgisApp::updateFilterLegend );
  connect( mMapCanvas, &QgsMapCanvas::renderErrorOccurred, badLayerIndicatorProvider, &QgsLayerTreeViewBadLayerIndicatorProvider::reportLayerError );
  connect( mMapCanvas, &QgsMapCanvas::renderErrorOccurred, mInfoBar, [this]( const QString & error, QgsMapLayer * layer )
  {
    mInfoBar->pushItem( new QgsMessageBarItem( layer->name(), QgsStringUtils::insertLinks( error ), Qgis::MessageLevel::Warning ) );
  } );
}

void QgisApp::setupLayerTreeViewFromSettings()
{
  QgsSettings s;

  QgsLayerTreeModel *model = mLayerTreeView->layerTreeModel();
  QFont fontLayer, fontGroup;
  fontLayer.setBold( true );
  fontGroup.setBold( false );
  model->setLayerTreeNodeFont( QgsLayerTreeNode::NodeLayer, fontLayer );
  model->setLayerTreeNodeFont( QgsLayerTreeNode::NodeGroup, fontGroup );
}


void QgisApp::updateNewLayerInsertionPoint()
{
  QgsLayerTreeRegistryBridge::InsertionPoint insertionPoint = layerTreeInsertionPoint();
  QgsProject::instance()->layerTreeRegistryBridge()->setLayerInsertionPoint( insertionPoint );
}

QgsLayerTreeRegistryBridge::InsertionPoint QgisApp::layerTreeInsertionPoint() const
{
  // defaults
  QgsLayerTreeGroup *insertGroup = mLayerTreeView->layerTreeModel()->rootGroup();
  QModelIndex current = mLayerTreeView->currentIndex();

  int index = 0;

  if ( current.isValid() )
  {
    index = current.row();

    QgsLayerTreeNode *currentNode = mLayerTreeView->currentNode();
    if ( currentNode )
    {
      // if the insertion point is actually a group, insert new layers into the group
      if ( QgsLayerTree::isGroup( currentNode ) )
      {
        // if the group is embedded go to the first non-embedded group, at worst the top level item
        QgsLayerTreeGroup *insertGroup = QgsLayerTreeUtils::firstGroupWithoutCustomProperty( QgsLayerTree::toGroup( currentNode ), QStringLiteral( "embedded" ) );

        return QgsLayerTreeRegistryBridge::InsertionPoint( insertGroup, 0 );
      }

      // otherwise just set the insertion point in front of the current node
      QgsLayerTreeNode *parentNode = currentNode->parent();
      if ( QgsLayerTree::isGroup( parentNode ) )
      {
        // if the group is embedded go to the first non-embedded group, at worst the top level item
        QgsLayerTreeGroup *parentGroup = QgsLayerTree::toGroup( parentNode );
        insertGroup = QgsLayerTreeUtils::firstGroupWithoutCustomProperty( parentGroup, QStringLiteral( "embedded" ) );
        if ( parentGroup != insertGroup )
          index = 0;
      }
    }
  }
  return QgsLayerTreeRegistryBridge::InsertionPoint( insertGroup, index );
}

void QgisApp::setGpsPanelConnection( QgsGpsConnection *connection )
{
  mpGpsWidget->setConnection( connection );
}

void QgisApp::autoSelectAddedLayer( QList<QgsMapLayer *> layers )
{
  if ( !layers.isEmpty() )
  {
    QgsLayerTreeLayer *nodeLayer = QgsProject::instance()->layerTreeRoot()->findLayer( layers[0]->id() );

    if ( !nodeLayer )
      return;

    QModelIndex index = mLayerTreeView->node2index( nodeLayer );
    mLayerTreeView->setCurrentIndex( index );
  }
}

void QgisApp::createMapTips()
{
  // Set up the timer for maptips. The timer is reset every time the mouse is moved
  mpMapTipsTimer = new QTimer( mMapCanvas );
  // connect the timer to the maptips slot
  connect( mpMapTipsTimer, &QTimer::timeout, this, &QgisApp::showMapTip );
  // set the delay to 0.850 seconds or time defined in the Settings
  // timer will be started next time the mouse moves
  QgsSettings settings;
  int timerInterval = settings.value( QStringLiteral( "qgis/mapTipsDelay" ), 850 ).toInt();
  mpMapTipsTimer->setInterval( timerInterval );
  mpMapTipsTimer->setSingleShot( true );

  // Create the maptips object
  mpMaptip = new QgsMapTip();
}

void QgisApp::setMapTipsDelay( int timerInterval )
{
  mpMapTipsTimer->setInterval( timerInterval );
}

void QgisApp::createDecorations()
{
  QgsDecorationTitle *decorationTitle = new QgsDecorationTitle( this );
  connect( mActionDecorationTitle, &QAction::triggered, decorationTitle, &QgsDecorationTitle::run );

  QgsDecorationCopyright *decorationCopyright = new QgsDecorationCopyright( this );
  connect( mActionDecorationCopyright, &QAction::triggered, decorationCopyright, &QgsDecorationCopyright::run );

  QgsDecorationImage *decorationImage = new QgsDecorationImage( this );
  connect( mActionDecorationImage, &QAction::triggered, decorationImage, &QgsDecorationImage::run );

  QgsDecorationNorthArrow *decorationNorthArrow = new QgsDecorationNorthArrow( this );
  connect( mActionDecorationNorthArrow, &QAction::triggered, decorationNorthArrow, &QgsDecorationNorthArrow::run );

  QgsDecorationScaleBar *decorationScaleBar = new QgsDecorationScaleBar( this );
  connect( mActionDecorationScaleBar, &QAction::triggered, decorationScaleBar, &QgsDecorationScaleBar::run );

  QgsDecorationGrid *decorationGrid = new QgsDecorationGrid( this );
  connect( mActionDecorationGrid, &QAction::triggered, decorationGrid, &QgsDecorationGrid::run );

  QgsDecorationLayoutExtent *decorationLayoutExtent = new QgsDecorationLayoutExtent( this );
  connect( mActionDecorationLayoutExtent, &QAction::triggered, decorationLayoutExtent, &QgsDecorationLayoutExtent::run );

  // add the decorations in a particular order so they are rendered in that order
  addDecorationItem( decorationGrid );
  addDecorationItem( decorationImage );
  addDecorationItem( decorationTitle );
  addDecorationItem( decorationCopyright );
  addDecorationItem( decorationNorthArrow );
  addDecorationItem( decorationScaleBar );
  addDecorationItem( decorationLayoutExtent );
  connect( mMapCanvas, &QgsMapCanvas::renderComplete, this, &QgisApp::renderDecorationItems );
  connect( this, &QgisApp::newProject, this, &QgisApp::projectReadDecorationItems );
  connect( this, &QgisApp::projectRead, this, &QgisApp::projectReadDecorationItems );
}

void QgisApp::renderDecorationItems( QPainter *p )
{
  QgsRenderContext context = QgsRenderContext::fromMapSettings( mMapCanvas->mapSettings() );
  context.setPainter( p );

  const auto constMDecorationItems = mDecorationItems;
  for ( QgsDecorationItem *item : constMDecorationItems )
  {
    item->render( mMapCanvas->mapSettings(), context );
  }
}

void QgisApp::projectReadDecorationItems()
{
  const auto constMDecorationItems = mDecorationItems;
  for ( QgsDecorationItem *item : constMDecorationItems )
  {
    item->projectRead();
  }
}

// Update project menu with the current list of recently accessed projects
void QgisApp::updateRecentProjectPaths()
{
  mRecentProjectsMenu->clear();

  const auto constMRecentProjects = mRecentProjects;
  for ( const QgsRecentProjectItemsModel::RecentProjectData &recentProject : constMRecentProjects )
  {
    QAction *action = mRecentProjectsMenu->addAction(
                        QStringLiteral( "%1 (%2)" )
                        .arg( recentProject.title != recentProject.path
                              ? recentProject.title
                              : QFileInfo( recentProject.path ).completeBaseName(), QDir::toNativeSeparators( recentProject.path )
                            ).replace( "&", "&&" )
                      );

    QgsProjectStorage *storage = QgsApplication::projectStorageRegistry()->projectStorageFromUri( recentProject.path );

    if ( storage )
    {
      QString path = storage->filePath( recentProject.path );
      // for geopackage projects, the path will be empty, if not valid
      if ( storage->type() == QLatin1String( "geopackage" ) && path.isEmpty() )
      {
        action->setEnabled( false );
        action->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mIndicatorBadLayer.svg" ) ) );
      }
    }
    else
    {
      bool exists = QFile::exists( recentProject.path );
      action->setEnabled( exists );
      if ( !exists )
        action->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mIndicatorBadLayer.svg" ) ) );
    }

    action->setData( recentProject.path );
    if ( recentProject.pin )
    {
      action->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/pin.svg" ) ) );
    }
  }

  std::vector< QgsNative::RecentProjectProperties > recentProjects;
  for ( const QgsRecentProjectItemsModel::RecentProjectData &recentProject : std::as_const( mRecentProjects ) )
  {
    QgsNative::RecentProjectProperties project;
    project.title = recentProject.title;
    project.fileName = QFileInfo( recentProject.path ).baseName();
    project.path = recentProject.path;
    project.name = project.title != project.path ? project.title : project.fileName;
    recentProjects.emplace_back( project );
  }
  QgsGui::nativePlatformInterface()->onRecentProjectsChanged( recentProjects );
}

// add this file to the recently opened/saved projects list
void QgisApp::saveRecentProjectPath( bool savePreviewImage, const QIcon &iconOverlay )
{
  // first, re-read the recent project paths. This prevents loss of recent
  // projects when multiple QGIS sessions are open
  readRecentProjects();

  // Get canonical absolute path
  QgsRecentProjectItemsModel::RecentProjectData projectData;
  projectData.path = QgsProject::instance()->absoluteFilePath();
  QString templateDirName = QgsSettings().value( QStringLiteral( "qgis/projectTemplateDir" ),
                            QString( QgsApplication::qgisSettingsDirPath() + "project_templates" ) ).toString();

  // We don't want the template path to appear in the recent projects list. Never.
  if ( projectData.path.startsWith( templateDirName ) )
    return;

  if ( projectData.path.isEmpty() )  // in case of custom project storage
    projectData.path = !QgsProject::instance()->fileName().isEmpty() ? QgsProject::instance()->fileName() : QgsProject::instance()->originalPath();
  projectData.title = QgsProject::instance()->title();
  if ( projectData.title.isEmpty() )
    projectData.title = !QgsProject::instance()->baseName().isEmpty() ? QgsProject::instance()->baseName() : QFileInfo( QgsProject::instance()->originalPath() ).completeBaseName();

  projectData.crs = QgsProject::instance()->crs().authid();

  int idx = mRecentProjects.indexOf( projectData );
  if ( idx != -1 )
    projectData.pin = mRecentProjects.at( idx ).pin;

  if ( savePreviewImage )
  {
    // Generate a unique file name
    QString fileName( QCryptographicHash::hash( ( projectData.path.toUtf8() ), QCryptographicHash::Md5 ).toHex() );
    QString previewDir = QStringLiteral( "%1/previewImages" ).arg( QgsApplication::qgisSettingsDirPath() );
    projectData.previewImagePath = QStringLiteral( "%1/%2.png" ).arg( previewDir, fileName );
    QDir().mkdir( previewDir );

    createPreviewImage( projectData.previewImagePath, iconOverlay );
  }
  else
  {
    if ( idx != -1 )
      projectData.previewImagePath = mRecentProjects.at( idx ).previewImagePath;
  }

  // Count the number of pinned items, those shouldn't affect trimming
  int pinnedCount = 0;
  int nonPinnedPos = 0;
  bool pinnedTop = true;
  for ( const QgsRecentProjectItemsModel::RecentProjectData &recentProject : std::as_const( mRecentProjects ) )
  {
    if ( recentProject.pin )
    {
      pinnedCount++;
      if ( pinnedTop )
      {
        nonPinnedPos++;
      }
    }
    else if ( pinnedTop )
    {
      pinnedTop = false;
    }
  }

  // If this file is already in the list, remove it
  mRecentProjects.removeAll( projectData );

  // Insert this file to the list
  mRecentProjects.insert( projectData.pin ? 0 : nonPinnedPos, projectData );

  const uint maxProjects = QgsSettings().value( QStringLiteral( "maxRecentProjects" ), 20, QgsSettings::App ).toUInt();

  // Keep the list to maxProjects items by trimming excess off the bottom
  // And remove the associated image
  while ( static_cast< uint >( mRecentProjects.count() ) > maxProjects + pinnedCount )
  {
    const QString previewImagePath = mRecentProjects.takeLast().previewImagePath;
    if ( QFileInfo::exists( previewImagePath ) )
      QFile( mRecentProjects.takeLast().previewImagePath ).remove();
  }

  // Persist the list
  saveRecentProjects();

  // Update menu list of paths
  updateRecentProjectPaths();

  // Update welcome page list
  // SG change
  //  if ( mWelcomePage )
  //    mWelcomePage->setRecentProjects( mRecentProjects );

} // QgisApp::saveRecentProjectPath

// Save recent projects list to settings
void QgisApp::saveRecentProjects()
{
  QgsSettings settings;

  settings.remove( QStringLiteral( "/UI/recentProjects" ) );
  int idx = 0;

  const auto constMRecentProjects = mRecentProjects;
  for ( const QgsRecentProjectItemsModel::RecentProjectData &recentProject : constMRecentProjects )
  {
    ++idx;
    settings.beginGroup( QStringLiteral( "UI/recentProjects/%1" ).arg( idx ) );
    settings.setValue( QStringLiteral( "title" ), recentProject.title );
    settings.setValue( QStringLiteral( "path" ), recentProject.path );
    settings.setValue( QStringLiteral( "previewImage" ), recentProject.previewImagePath );
    settings.setValue( QStringLiteral( "crs" ), recentProject.crs );
    settings.setValue( QStringLiteral( "pin" ), recentProject.pin );
    settings.endGroup();
  }
}

// Update project menu with the project templates
void QgisApp::updateProjectFromTemplates()
{
  // get list of project files in template dir
  QgsSettings settings;
  QString templateDirName = settings.value( QStringLiteral( "qgis/projectTemplateDir" ),
                            QString( QgsApplication::qgisSettingsDirPath() + "project_templates" ) ).toString();
  QDir templateDir( templateDirName );
  QStringList filters( QStringLiteral( "*.qgs" ) );
  filters << QStringLiteral( "*.qgz" );
  templateDir.setNameFilters( filters );
  QStringList templateFiles = templateDir.entryList( filters );

  // Remove existing entries
  mProjectFromTemplateMenu->clear();

  // Add entries
  const auto constTemplateFiles = templateFiles;
  for ( const QString &templateFile : constTemplateFiles )
  {
    mProjectFromTemplateMenu->addAction( templateFile );
  }

  // add <blank> entry, which loads a blank template (regardless of "default template")
  if ( settings.value( QStringLiteral( "qgis/newProjectDefault" ), QVariant( false ) ).toBool() )
    mProjectFromTemplateMenu->addAction( tr( "< Blank >" ) );

} // QgisApp::updateProjectFromTemplates

void QgisApp::saveWindowState()
{
  // store window and toolbar positions
  QgsSettings settings;
  // store the toolbar/dock widget settings using Qt4 settings API
  settings.setValue( QStringLiteral( "UI/state" ), saveState() );

  // store window geometry
  settings.setValue( QStringLiteral( "UI/geometry" ), saveGeometry() );

  QgsPluginRegistry::instance()->unloadAll();
}

#include "ui_defaults.h"

void QgisApp::restoreWindowState()
{
  // restore the toolbar and dock widgets positions using Qt4 settings API
  QgsSettings settings;
#if 0
  // because of Qt regression: https://bugreports.qt.io/browse/QTBUG-89034
  // we have to wait till dialog is first shown to try to restore dock geometry or it's not correctly restored
  // so this code was moved to showEvent for now...
  if ( !restoreState( settings.value( QStringLiteral( "UI/state" ), QByteArray::fromRawData( reinterpret_cast< const char * >( defaultUIstate ), sizeof defaultUIstate ) ).toByteArray() ) )
  {
    QgsDebugMsg( QStringLiteral( "restore of UI state failed" ) );
  }
#endif

  if ( settings.value( QStringLiteral( "UI/hidebrowser" ), false ).toBool() )
  {
    mBrowserWidget->hide();
    mBrowserWidget2->hide();
    settings.remove( QStringLiteral( "UI/hidebrowser" ) );
  }

  // restore window geometry
  if ( !restoreGeometry( settings.value( QStringLiteral( "UI/geometry" ) ).toByteArray() ) )
  {
    QgsDebugMsg( QStringLiteral( "restore of UI geometry failed" ) );
    // default to 80% of screen size, at 10% from top left corner
    resize( QDesktopWidget().availableGeometry( this ).size() * 0.8 );
    QSize pos = QDesktopWidget().availableGeometry( this ).size() * 0.1;
    move( pos.width(), pos.height() );
  }

}
///////////// END OF GUI SETUP ROUTINES ///////////////
void QgisApp::sponsors()
{
  QgsSettings settings;
  QString qgisSponsorsUrl = settings.value( QStringLiteral( "qgis/qgisSponsorsUrl" ),
                            tr( "https://qgis.org/en/site/about/sustaining_members.html" ) ).toString();
  openURL( qgisSponsorsUrl, false );
}

void QgisApp::about()
{
  static QgsAbout *sAbt = nullptr;
  if ( !sAbt )
  {
    sAbt = new QgsAbout( this );
    QString versionString = QStringLiteral( "<html><body><div align='center'><table width='100%'>" );

    versionString += QStringLiteral( "<tr><td>%1</td><td>%2</td><td>" ).arg( tr( "QGIS version" ), Qgis::version() );

    if ( QString( Qgis::devVersion() ) == QLatin1String( "exported" ) )
    {
      versionString += tr( "QGIS code branch" );
      if ( Qgis::version().endsWith( QLatin1String( "Master" ) ) )
      {
        versionString += QLatin1String( "</td><td><a href=\"https://github.com/qgis/QGIS/tree/master\">master</a></td>" );
      }
      else
      {
        versionString += QStringLiteral( "</td><td><a href=\"https://github.com/qgis/QGIS/tree/release-%1_%2\">Release %1.%2</a></td>" )
                         .arg( Qgis::versionInt() / 10000 ).arg( Qgis::versionInt() / 100 % 100 );
      }
    }
    else
    {
      versionString += QStringLiteral( "%1</td><td><a href=\"https://github.com/qgis/QGIS/commit/%2\">%2</a></td>" ).arg( tr( "QGIS code revision" ) ).arg( Qgis::devVersion() );
    }
    versionString += QLatin1String( "</tr><tr>" );

    // Qt version
    const QString qtVersionCompiled{ QT_VERSION_STR };
    const QString qtVersionRunning{ qVersion() };
    if ( qtVersionCompiled != qtVersionRunning )
    {
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Compiled against Qt" ), qtVersionCompiled );
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Running against Qt" ), qtVersionRunning );
    }
    else
    {
      versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "Qt version" ), qtVersionCompiled );
    }
    versionString += QLatin1String( "</tr><tr>" );

    // Python version
    versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "Python version" ), PYTHON_VERSION );
    versionString += QLatin1String( "</tr><tr>" );

    // GDAL version
    const QString gdalVersionCompiled { GDAL_RELEASE_NAME };
    const QString gdalVersionRunning { GDALVersionInfo( "RELEASE_NAME" ) };
    if ( gdalVersionCompiled != gdalVersionRunning )
    {
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Compiled against GDAL/OGR" ), gdalVersionCompiled );
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Running against GDAL/OGR" ), gdalVersionRunning );
    }
    else
    {
      versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "GDAL/OGR version" ), gdalVersionCompiled );
    }
    versionString += QLatin1String( "</tr><tr>" );

    // proj
    PJ_INFO info = proj_info();
    const QString projVersionCompiled { QStringLiteral( "%1.%2.%3" ).arg( PROJ_VERSION_MAJOR ).arg( PROJ_VERSION_MINOR ).arg( PROJ_VERSION_PATCH ) };
    const QString projVersionRunning { info.version };
    if ( projVersionCompiled != projVersionRunning )
    {
      versionString += QStringLiteral( "<td>%1</td><td>%2.%3.%4</td>" ).arg( tr( "Compiled against PROJ" ), projVersionCompiled );
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Running against PROJ" ), projVersionRunning );
    }
    else
    {
      versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "PROJ version" ), projVersionCompiled );
    }
    versionString += QLatin1String( "</tr><tr>" );

    // CRS database versions
    versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2 (%3)</td>" ).arg( tr( "EPSG Registry database version" ), QgsProjUtils::epsgRegistryVersion(), QgsProjUtils::epsgRegistryDate().toString( Qt::ISODate ) );
    versionString += QLatin1String( "</tr><tr>" );

    // GEOS version
    const QString geosVersionCompiled { GEOS_CAPI_VERSION };
    //const QString geosVersionRunning { GEOSversion() }; // SG add geos compile error
    if ( !geosVersionCompiled.isEmpty() /*!= geosVersionRunning */)
    {
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Compiled against GEOS" ), geosVersionCompiled );
      //versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Running against GEOS" ), geosVersionRunning );
    }
    else
    {
      versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "GEOS version" ), geosVersionCompiled );
    }
    versionString += QLatin1String( "</tr><tr>" );

    // SQLite version
    const QString sqliteVersionCompiled { SQLITE_VERSION };
    const QString sqliteVersionRunning { sqlite3_libversion() };
    if ( sqliteVersionCompiled != sqliteVersionRunning )
    {
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Compiled against SQLite" ), sqliteVersionCompiled );
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Running against SQLite" ), sqliteVersionRunning );
    }
    else
    {
      versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "SQLite version" ), sqliteVersionCompiled );
    }
    versionString += QLatin1String( "</tr><tr>" );

    // PDAL
#ifdef HAVE_PDAL_QGIS
    const QString pdalVersionCompiled { PDAL_VERSION };
#if PDAL_VERSION_MAJOR_INT > 1 || (PDAL_VERSION_MAJOR_INT == 1 && PDAL_VERSION_MINOR_INT >= 7)
    const QString pdalVersionRunningRaw { QString::fromStdString( pdal::Config::fullVersionString() ) };
#else
    const QString pdalVersionRunningRaw { QString::fromStdString( pdal::GetFullVersionString() ) };
#endif
    const QRegularExpression pdalVersionRx { QStringLiteral( "(\\d+\\.\\d+\\.\\d+)" )};
    const QRegularExpressionMatch pdalVersionMatch{ pdalVersionRx.match( pdalVersionRunningRaw ) };
    const QString pdalVersionRunning{ pdalVersionMatch.hasMatch() ? pdalVersionMatch.captured( 1 ) : pdalVersionRunningRaw };
    if ( pdalVersionCompiled != pdalVersionRunning )
    {
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Compiled against PDAL" ), pdalVersionCompiled );
      versionString += QStringLiteral( "<td>%1</td><td>%2</td>" ).arg( tr( "Running against PDAL" ), pdalVersionRunning );
    }
    else
    {
      versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "PDAL version" ), pdalVersionCompiled );
    }
    versionString += QLatin1String( "</tr><tr>" );
#endif

    // postgres
    versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">" ).arg( tr( "PostgreSQL client version" ) );
#ifdef HAVE_POSTGRESQL
    versionString += QStringLiteral( PG_VERSION );
#else
    versionString += tr( "No support" );
#endif
    versionString += QLatin1String( "</td></tr><tr>" );

    // spatialite
    versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">" ).arg( tr( "SpatiaLite version" ) );
#ifdef HAVE_SPATIALITE
    versionString += QStringLiteral( "%1</td>" ).arg( spatialite_version() );
#else
    versionString += tr( "No support" );
#endif
    versionString += QLatin1String( "</td></tr><tr>" );

    // QWT
    versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "QWT version" ), QWT_VERSION_STR );
    versionString += QLatin1String( "</tr><tr>" );

    // QScintilla
    versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "QScintilla2 version" ), QSCINTILLA_VERSION_STR );
    versionString += QLatin1String( "</tr><tr>" );

    // Operating system
    versionString += QStringLiteral( "<td>%1</td><td colspan=\"3\">%2</td>" ).arg( tr( "OS version" ), QSysInfo::prettyProductName() );
    versionString += QLatin1String( "</tr><tr>" );

#ifdef QGISDEBUG
    versionString += QLatin1String( "</tr><tr>" );
    versionString += QStringLiteral( "<td colspan=\"4\"><i>%1</i></td>" ).arg( tr( "This copy of QGIS writes debugging output." ) );
    versionString += QLatin1String( "</tr><tr>" );
#endif

#ifdef WITH_BINDINGS
    if ( mPythonUtils && mPythonUtils->isEnabled() )
    {
      versionString += QStringLiteral( "</tr><tr><td colspan=\"4\">%1</td>" ).arg( tr( "Active Python plugins" ) );
      const QStringList activePlugins = mPythonUtils->listActivePlugins();
      for ( const QString &plugin : activePlugins )
      {
        const QString version = mPythonUtils->getPluginMetadata( plugin, QStringLiteral( "version" ) );
        versionString += QStringLiteral( "</tr><tr><td>%1</td><td colspan=\"3\">%2</td>" ).arg( plugin, version );
      }
    }
#endif

    versionString += QLatin1String( "</tr></table></div></body></html>" );

    sAbt->setVersion( versionString );
  }
  sAbt->show();
  sAbt->raise();
  sAbt->activateWindow();
}

void QgisApp::addLayerDefinition()
{
  QgsSettings settings;
  QString lastUsedDir = settings.value( QStringLiteral( "UI/lastQLRDir" ), QDir::homePath() ).toString();

  QString path = QFileDialog::getOpenFileName( this, QStringLiteral( "Add Layer Definition File" ), lastUsedDir, QStringLiteral( "*.qlr" ) );
  if ( path.isEmpty() )
    return;

  QFileInfo fi( path );
  settings.setValue( QStringLiteral( "UI/lastQLRDir" ), fi.path() );

  openLayerDefinition( path );
}

QString QgisApp::crsAndFormatAdjustedLayerUri( const QString &uri, const QStringList &supportedCrs, const QStringList &supportedFormats ) const
{
  QString newuri = uri;

  // Adjust layer CRS to project CRS
  QgsCoordinateReferenceSystem testCrs;
  const auto constSupportedCrs = supportedCrs;
  for ( const QString &c : constSupportedCrs )
  {
    testCrs.createFromOgcWmsCrs( c );
    if ( testCrs == mMapCanvas->mapSettings().destinationCrs() )
    {
      newuri.replace( QRegExp( "crs=[^&]+" ), "crs=" + c );
      QgsDebugMsgLevel( QStringLiteral( "Changing layer crs to %1, new uri: %2" ).arg( c, uri ), 2 );
      break;
    }
  }

  // Use the last used image format
  QString lastImageEncoding = QgsSettings().value( QStringLiteral( "/qgis/lastWmsImageEncoding" ), "image/png" ).toString();
  const auto constSupportedFormats = supportedFormats;
  for ( const QString &fmt : constSupportedFormats )
  {
    if ( fmt == lastImageEncoding )
    {
      newuri.replace( QRegExp( "format=[^&]+" ), "format=" + fmt );
      QgsDebugMsgLevel( QStringLiteral( "Changing layer format to %1, new uri: %2" ).arg( fmt, uri ), 2 );
      break;
    }
  }
  return newuri;
}

bool QgisApp::addVectorLayers( const QStringList &layerQStringList, const QString &enc, const QString &dataSourceType )
{
  return addVectorLayersPrivate( layerQStringList, enc, dataSourceType );
}

bool QgisApp::addVectorLayersPrivate( const QStringList &layers, const QString &enc, const QString &dataSourceType, const bool guiWarning )
{
  //note: this method ONLY supports vector layers from the OGR provider!

  QgsCanvasRefreshBlocker refreshBlocker;

  QList<QgsMapLayer *> layersToAdd;
  QList<QgsMapLayer *> addedLayers;
  QgsSettings settings;
  bool userAskedToAddLayers = false;

  for ( const QString &layerUri : layers )
  {
    const QString uri = layerUri.trimmed();
    QString baseName;
    if ( dataSourceType == QLatin1String( "file" ) )
    {
      QString srcWithoutLayername( uri );
      int posPipe = srcWithoutLayername.indexOf( '|' );
      if ( posPipe >= 0 )
        srcWithoutLayername.resize( posPipe );
      baseName = QgsProviderUtils::suggestLayerNameFromFilePath( srcWithoutLayername );

      // if needed prompt for zipitem layers
      QString vsiPrefix = QgsZipItem::vsiPrefix( uri );
      if ( ! uri.startsWith( QLatin1String( "/vsi" ), Qt::CaseInsensitive ) &&
           ( vsiPrefix == QLatin1String( "/vsizip/" ) || vsiPrefix == QLatin1String( "/vsitar/" ) ) )
      {
        if ( askUserForZipItemLayers( uri, { QgsMapLayerType::VectorLayer } ) )
          continue;
      }
    }
    else if ( dataSourceType == QLatin1String( "database" ) )
    {
      // Try to extract the database name and use it as base name
      // sublayers names (if any) will be appended to the layer name
      const QVariantMap parts( QgsProviderRegistry::instance()->decodeUri( QStringLiteral( "ogr" ), uri ) );
      if ( parts.value( QStringLiteral( "databaseName" ) ).isValid() )
        baseName = parts.value( QStringLiteral( "databaseName" ) ).toString();
      else
        baseName = uri;
    }
    else //directory //protocol
    {
      baseName = QgsProviderUtils::suggestLayerNameFromFilePath( uri );
    }

    if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
    {
      baseName = QgsMapLayer::formatLayerName( baseName );
    }

    QgsDebugMsgLevel( "completeBaseName: " + baseName, 2 );
    const bool isVsiCurl { uri.startsWith( QLatin1String( "/vsicurl" ), Qt::CaseInsensitive ) };
    const auto scheme { QUrl( uri ).scheme() };
    const bool isRemoteUrl { scheme.startsWith( QLatin1String( "http" ) ) || scheme == QLatin1String( "ftp" ) };

    std::unique_ptr< QgsTemporaryCursorOverride > cursorOverride;
    if ( isVsiCurl || isRemoteUrl )
    {
      cursorOverride = std::make_unique< QgsTemporaryCursorOverride >( Qt::WaitCursor );
      visibleMessageBar()->pushInfo( tr( "Remote layer" ), tr( "loading %1, please wait …" ).arg( uri ) );
      qApp->processEvents();
    }

    QList< QgsProviderSublayerDetails > sublayers = QgsProviderRegistry::instance()->providerMetadata( QStringLiteral( "ogr" ) )->querySublayers( uri, Qgis::SublayerQueryFlag::IncludeSystemTables );
    // filter out non-vector sublayers
    sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), []( const QgsProviderSublayerDetails & sublayer )
    {
      return sublayer.type() != QgsMapLayerType::VectorLayer;
    } ), sublayers.end() );

    cursorOverride.reset();

    const QVariantMap uriParts = QgsProviderRegistry::instance()->decodeUri( QStringLiteral( "ogr" ), uri );
    const QString path = uriParts.value( QStringLiteral( "path" ) ).toString();

    if ( !sublayers.empty() )
    {
      userAskedToAddLayers = true;

      const bool detailsAreIncomplete = QgsProviderUtils::sublayerDetailsAreIncomplete( sublayers, QgsProviderUtils::SublayerCompletenessFlag::IgnoreUnknownFeatureCount );
      const bool singleSublayerOnly = sublayers.size() == 1;
      QString groupName;

      if ( !singleSublayerOnly || detailsAreIncomplete )
      {
        // ask user for sublayers (unless user settings dictate otherwise!)
        switch ( shouldAskUserForSublayers( sublayers ) )
        {
          case SublayerHandling::AskUser:
          {
            // prompt user for sublayers
            QgsProviderSublayersDialog dlg( uri, path, sublayers, {QgsMapLayerType::VectorLayer}, this );

            if ( dlg.exec() )
              sublayers = dlg.selectedLayers();
            else
              sublayers.clear(); // dialog was canceled, so don't add any sublayers
            groupName = dlg.groupName();
            break;
          }

          case SublayerHandling::LoadAll:
          {
            if ( detailsAreIncomplete )
            {
              // requery sublayers, resolving geometry types
              sublayers = QgsProviderRegistry::instance()->querySublayers( uri, Qgis::SublayerQueryFlag::ResolveGeometryType );
              // filter out non-vector sublayers
              sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), []( const QgsProviderSublayerDetails & sublayer )
              {
                return sublayer.type() != QgsMapLayerType::VectorLayer;
              } ), sublayers.end() );
            }
            break;
          }

          case SublayerHandling::AbortLoading:
            sublayers.clear(); // don't add any sublayers
            break;
        };
      }
      else if ( detailsAreIncomplete )
      {
        // requery sublayers, resolving geometry types
        sublayers = QgsProviderRegistry::instance()->querySublayers( uri, Qgis::SublayerQueryFlag::ResolveGeometryType );
        // filter out non-vector sublayers
        sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), []( const QgsProviderSublayerDetails & sublayer )
        {
          return sublayer.type() != QgsMapLayerType::VectorLayer;
        } ), sublayers.end() );
      }

      // now add sublayers
      if ( !sublayers.empty() )
      {
        addedLayers << addSublayers( sublayers, baseName, groupName );
      }

    }
    else
    {
      QString msg = tr( "%1 is not a valid or recognized data source." ).arg( uri );
      // If the failed layer was a vsicurl type, give the user a chance to try the normal download.
      if ( isVsiCurl &&
           QMessageBox::question( this, tr( "Invalid Data Source" ),
                                  tr( "Download with \"Protocol\" source type has failed, do you want to try the \"File\" source type?" ) ) == QMessageBox::Yes )
      {
        QString fileUri = uri;
        fileUri.replace( QLatin1String( "/vsicurl/" ), " " );
        return addVectorLayersPrivate( QStringList() << fileUri, enc, dataSourceType, guiWarning );
      }
      else if ( guiWarning )
      {
        visibleMessageBar()->pushMessage( tr( "Invalid Data Source" ), msg, Qgis::MessageLevel::Critical );
      }
    }
  }

  // make sure at least one layer was successfully added
  if ( layersToAdd.isEmpty() )
  {
    // we also return true if we asked the user for sublayers, but they choose none. In this case nothing
    // went wrong, so we shouldn't return false and cause GUI warnings to appear
    return userAskedToAddLayers || !addedLayers.isEmpty();
  }

  // Register this layer with the layers registry
  QgsProject::instance()->addMapLayers( layersToAdd );
  for ( QgsMapLayer *l : std::as_const( layersToAdd ) )
  {
    if ( !enc.isEmpty() )
    {
      if ( QgsVectorLayer *vl = qobject_cast< QgsVectorLayer * >( l ) )
        vl->setProviderEncoding( enc );
    }

    askUserForDatumTransform( l->crs(), QgsProject::instance()->crs(), l );
    postProcessAddedLayer( l );
  }
  activateDeactivateLayerRelatedActions( activeLayer() );

  return true;
}

QgsMeshLayer *QgisApp::addMeshLayer( const QString &url, const QString &baseName, const QString &providerKey )
{
  return addLayerPrivate< QgsMeshLayer >( QgsMapLayerType::MeshLayer, url, baseName, providerKey, true );
}

QList< QgsMapLayer * > QgisApp::addSublayers( const QList<QgsProviderSublayerDetails> &layers, const QString &baseName, const QString &groupName )
{
  QgsLayerTreeGroup *group = nullptr;
  if ( !groupName.isEmpty() )
  {
    group = QgsProject::instance()->layerTreeRoot()->insertGroup( 0, groupName );
  }

  QgsSettings settings;
  const bool formatLayerNames = settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool();

  // if we aren't adding to a group, we need to add the layers in reverse order so that they maintain the correct
  // order in the layer tree!
  QList<QgsProviderSublayerDetails> sortedLayers = layers;
  if ( groupName.isEmpty() )
  {
    std::reverse( sortedLayers.begin(), sortedLayers.end() );
  }

  QList< QgsMapLayer * > result;
  result.reserve( sortedLayers.size() );

  for ( const QgsProviderSublayerDetails &sublayer : std::as_const( sortedLayers ) )
  {
    QgsProviderSublayerDetails::LayerOptions options( QgsProject::instance()->transformContext() );
    options.loadDefaultStyle = false;

    std::unique_ptr<QgsMapLayer> layer( sublayer.toLayer( options ) );
    if ( !layer )
      continue;

    QgsMapLayer *ml = layer.get();
    // if we aren't adding to a group, then we're iterating the layers in the reverse order
    // so account for that in the returned list of layers
    if ( groupName.isEmpty() )
      result.insert( 0, ml );
    else
      result << ml;

    QString layerName = layer->name();
    if ( formatLayerNames )
    {
      layerName = QgsMapLayer::formatLayerName( layerName );
    }

    // if user has opted to add sublayers to a group, then we don't need to include the
    // filename in the layer's name, because the group is already titled with the filename.
    // But otherwise, we DO include the file name so that users can differentiate the source
    // when multiple layers are loaded from a GPX file or similar (refs https://github.com/qgis/QGIS/issues/37551)
    if ( group )
    {
      if ( !layerName.isEmpty() )
        layer->setName( layerName );
      else if ( !baseName.isEmpty() )
        layer->setName( baseName );
      QgsProject::instance()->addMapLayer( layer.release(), false );
      group->addLayer( ml );
    }
    else
    {
      if ( layerName != baseName && !layerName.isEmpty() && !baseName.isEmpty() )
        layer->setName( QStringLiteral( "%1 — %2" ).arg( baseName, layerName ) );
      else if ( !layerName.isEmpty() )
        layer->setName( layerName );
      else if ( !baseName.isEmpty() )
        layer->setName( baseName );
      QgsProject::instance()->addMapLayer( layer.release() );
    }

    askUserForDatumTransform( ml->crs(), QgsProject::instance()->crs(), ml );
    postProcessAddedLayer( ml );
  }

  if ( group )
  {
    // Respect if user don't want the new group of layers visible.
    QgsSettings settings;
    const bool newLayersVisible = settings.value( QStringLiteral( "/qgis/new_layers_visible" ), true ).toBool();
    if ( !newLayersVisible )
      group->setItemVisibilityCheckedRecursive( newLayersVisible );
  }

  return result;
}

void QgisApp::postProcessAddedLayer( QgsMapLayer *layer )
{
  switch ( layer->type() )
  {
    case QgsMapLayerType::VectorLayer:
    case QgsMapLayerType::RasterLayer:
    {
      bool ok = false;
      layer->loadDefaultStyle( ok );
      layer->loadDefaultMetadata( ok );
      break;
    }

    case QgsMapLayerType::PluginLayer:
      break;

    case QgsMapLayerType::MeshLayer:
    {
      QgsMeshLayer *meshLayer = qobject_cast< QgsMeshLayer *>( layer );
      QDateTime referenceTime = QgsProject::instance()->timeSettings()->temporalRange().begin();
      if ( !referenceTime.isValid() ) // If project reference time is invalid, use current date
        referenceTime = QDateTime( QDate::currentDate(), QTime( 0, 0, 0 ), Qt::UTC );

      if ( meshLayer->dataProvider() && !qobject_cast< QgsMeshLayerTemporalProperties * >( meshLayer->temporalProperties() )->referenceTime().isValid() )
        qobject_cast< QgsMeshLayerTemporalProperties * >( meshLayer->temporalProperties() )->setReferenceTime( referenceTime, meshLayer->dataProvider()->temporalCapabilities() );

      bool ok = false;
      meshLayer->loadDefaultStyle( ok );
      meshLayer->loadDefaultMetadata( ok );
      break;
    }

    case QgsMapLayerType::VectorTileLayer:
    {
      bool ok = false;
      QString error = layer->loadDefaultStyle( ok );
      if ( !ok )
        visibleMessageBar()->pushMessage( tr( "Error loading style" ), error, Qgis::MessageLevel::Warning );
      error = layer->loadDefaultMetadata( ok );
      if ( !ok )
        visibleMessageBar()->pushMessage( tr( "Error loading layer metadata" ), error, Qgis::MessageLevel::Warning );

      break;
    }

    case QgsMapLayerType::AnnotationLayer:
    case QgsMapLayerType::GroupLayer:
      break;

    case QgsMapLayerType::PointCloudLayer:
    {
      bool ok = false;
      layer->loadDefaultStyle( ok );
      layer->loadDefaultMetadata( ok );

#ifdef HAVE_3D
      if ( !layer->renderer3D() )
      {
        QgsPointCloudLayer *pcLayer = qobject_cast< QgsPointCloudLayer * >( layer );
        // for point clouds we default to a 3d renderer. it just makes sense :)
        std::unique_ptr< QgsPointCloudLayer3DRenderer > renderer3D = Qgs3DUtils::convert2DPointCloudRendererTo3D( pcLayer->renderer() );
        if ( renderer3D )
          layer->setRenderer3D( renderer3D.release() );
        else
        {
          // maybe waiting on an index...
          if ( pcLayer->dataProvider()->indexingState() != QgsPointCloudDataProvider::Indexed )
          {
            QPointer< QgsPointCloudLayer > layerPointer( pcLayer );
            connect( pcLayer->dataProvider(), &QgsPointCloudDataProvider::indexGenerationStateChanged, this, [layerPointer]( QgsPointCloudDataProvider::PointCloudIndexGenerationState state )
            {
              if ( !layerPointer || state != QgsPointCloudDataProvider::Indexed )
                return;

              std::unique_ptr< QgsPointCloudLayer3DRenderer > renderer3D = Qgs3DUtils::convert2DPointCloudRendererTo3D( layerPointer->renderer() );
              if ( renderer3D )
                layerPointer->setRenderer3D( renderer3D.release() );
            } );
          }
        }
      }
#endif
      break;
    }
  }
}

QgsVectorTileLayer *QgisApp::addVectorTileLayer( const QString &url, const QString &baseName )
{
  return addVectorTileLayerPrivate( url, baseName );
}

QgsPointCloudLayer *QgisApp::addPointCloudLayer( const QString &url, const QString &baseName, const QString &providerKey )
{
  return addPointCloudLayerPrivate( url, baseName, providerKey );
}

QgsVectorTileLayer *QgisApp::addVectorTileLayerPrivate( const QString &url, const QString &baseName, const bool guiWarning )
{
  QgsCanvasRefreshBlocker refreshBlocker;
  QgsSettings settings;

  QString base( baseName );

  if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
  {
    base = QgsMapLayer::formatLayerName( base );
  }

  QgsDebugMsgLevel( "completeBaseName: " + base, 2 );

  // create the layer
  const QgsVectorTileLayer::LayerOptions options( QgsProject::instance()->transformContext() );
  std::unique_ptr<QgsVectorTileLayer> layer( new QgsVectorTileLayer( url, base, options ) );

  if ( !layer || !layer->isValid() )
  {
    if ( guiWarning )
    {
      QString msg = tr( "%1 is not a valid or recognized data source." ).arg( url );
      visibleMessageBar()->pushMessage( tr( "Invalid Data Source" ), msg, Qgis::MessageLevel::Critical );
    }

    // since the layer is bad, stomp on it
    return nullptr;
  }

  postProcessAddedLayer( layer.get() );

  QgsProject::instance()->addMapLayer( layer.get() );
  activateDeactivateLayerRelatedActions( activeLayer() );

  return layer.release();
}

QgsPointCloudLayer *QgisApp::addPointCloudLayerPrivate( const QString &uri, const QString &baseName, const QString &providerKey, bool guiWarning )
{
  QgsCanvasRefreshBlocker refreshBlocker;
  QgsSettings settings;

  QString base( baseName );

  if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
  {
    base = QgsMapLayer::formatLayerName( base );
  }

  QgsDebugMsgLevel( "completeBaseName: " + base, 2 );

  // create the layer
  std::unique_ptr<QgsPointCloudLayer> layer( new QgsPointCloudLayer( uri, base, providerKey ) );

  if ( !layer || !layer->isValid() )
  {
    if ( guiWarning )
    {
      QString msg = tr( "%1 is not a valid or recognized data source." ).arg( uri );
      visibleMessageBar()->pushMessage( tr( "Invalid Data Source" ), msg, Qgis::MessageLevel::Critical );
    }

    // since the layer is bad, stomp on it
    return nullptr;
  }

  postProcessAddedLayer( layer.get() );


  QgsProject::instance()->addMapLayer( layer.get() );
  activateDeactivateLayerRelatedActions( activeLayer() );

  return layer.release();
}

bool QgisApp::askUserForZipItemLayers( const QString &path, const QList< QgsMapLayerType > &acceptableTypes )
{
  // query sublayers
  QList< QgsProviderSublayerDetails > sublayers = QgsProviderRegistry::instance()->querySublayers( path, Qgis::SublayerQueryFlag::IncludeSystemTables );

  // filter out non-matching sublayers
  sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), [acceptableTypes]( const QgsProviderSublayerDetails & sublayer )
  {
    return !acceptableTypes.empty() && !acceptableTypes.contains( sublayer.type() );
  } ), sublayers.end() );

  if ( sublayers.empty() )
    return false;

  const bool detailsAreIncomplete = QgsProviderUtils::sublayerDetailsAreIncomplete( sublayers, QgsProviderUtils::SublayerCompletenessFlag::IgnoreUnknownFeatureCount );
  const bool singleSublayerOnly = sublayers.size() == 1;
  QString groupName;

  if ( !singleSublayerOnly || detailsAreIncomplete )
  {
    // ask user for sublayers (unless user settings dictate otherwise!)
    switch ( shouldAskUserForSublayers( sublayers ) )
    {
      case SublayerHandling::AskUser:
      {
        // prompt user for sublayers
        QgsProviderSublayersDialog dlg( path, path, sublayers, acceptableTypes, this );

        if ( dlg.exec() )
          sublayers = dlg.selectedLayers();
        else
          sublayers.clear(); // dialog was canceled, so don't add any sublayers
        groupName = dlg.groupName();
        break;
      }

      case SublayerHandling::LoadAll:
      {
        if ( detailsAreIncomplete )
        {
          // requery sublayers, resolving geometry types
          sublayers = QgsProviderRegistry::instance()->querySublayers( path, Qgis::SublayerQueryFlag::ResolveGeometryType );
          sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), [acceptableTypes]( const QgsProviderSublayerDetails & sublayer )
          {
            return !acceptableTypes.empty() && !acceptableTypes.contains( sublayer.type() );
          } ), sublayers.end() );
        }
        break;
      }

      case SublayerHandling::AbortLoading:
        sublayers.clear(); // don't add any sublayers
        break;
    };
  }
  else if ( detailsAreIncomplete )
  {
    // requery sublayers, resolving geometry types
    sublayers = QgsProviderRegistry::instance()->querySublayers( path, Qgis::SublayerQueryFlag::ResolveGeometryType );
    sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), [acceptableTypes]( const QgsProviderSublayerDetails & sublayer )
    {
      return !acceptableTypes.empty() && !acceptableTypes.contains( sublayer.type() );
    } ), sublayers.end() );
  }

  // now add sublayers
  if ( !sublayers.empty() )
  {
    QgsCanvasRefreshBlocker refreshBlocker;
    QgsSettings settings;

    QString base = QgsProviderUtils::suggestLayerNameFromFilePath( path );
    if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
    {
      base = QgsMapLayer::formatLayerName( base );
    }

    addSublayers( sublayers, base, groupName );
    activateDeactivateLayerRelatedActions( activeLayer() );
  }

  return true;
}

QgisApp::SublayerHandling QgisApp::shouldAskUserForSublayers( const QList<QgsProviderSublayerDetails> &layers, bool hasNonLayerItems ) const
{
  if ( hasNonLayerItems )
    return SublayerHandling::AskUser;

  QgsSettings settings;
  const Qgis::SublayerPromptMode promptLayers = settings.enumValue( QStringLiteral( "qgis/promptForSublayers" ), Qgis::SublayerPromptMode::AlwaysAsk );

  switch ( promptLayers )
  {
    case Qgis::SublayerPromptMode::AlwaysAsk:
      return SublayerHandling::AskUser;

    case Qgis::SublayerPromptMode::AskExcludingRasterBands:
    {
      // if any non-raster layers are found, we ask the user. Otherwise we load all
      for ( const QgsProviderSublayerDetails &sublayer : layers )
      {
        if ( sublayer.type() != QgsMapLayerType::RasterLayer )
          return SublayerHandling::AskUser;
      }
      return SublayerHandling::LoadAll;
    }

    case Qgis::SublayerPromptMode::NeverAskSkip:
      return SublayerHandling::AbortLoading;

    case Qgis::SublayerPromptMode::NeverAskLoadAll:
      return SublayerHandling::LoadAll;
  }

  return SublayerHandling::AskUser;
}

void QgisApp::addDatabaseLayers( QStringList const &layerPathList, QString const &providerKey )
{
  QList<QgsMapLayer *> myList;

  if ( layerPathList.empty() )
  {
    // no layers to add so bail out, but
    // allow mMapCanvas to handle events
    // first
    return;
  }

  QgsCanvasRefreshBlocker refreshBlocker;

  QApplication::setOverrideCursor( Qt::WaitCursor );

  const auto constLayerPathList = layerPathList;
  for ( const QString &layerPath : constLayerPathList )
  {
    // create the layer
    QgsDataSourceUri uri( layerPath );

    QgsVectorLayer::LayerOptions options { QgsProject::instance()->transformContext() };
    options.loadDefaultStyle = false;
    QgsVectorLayer *layer = new QgsVectorLayer( uri.uri( false ), uri.table(), providerKey, options );
    Q_CHECK_PTR( layer );

    if ( ! layer )
    {
      QApplication::restoreOverrideCursor();

      // XXX insert meaningful whine to the user here
      return;
    }

    if ( layer->isValid() )
    {
      // add to list of layers to register
      //with the central layers registry
      myList << layer;
    }
    else
    {
      QgsMessageLog::logMessage( tr( "%1 is an invalid layer - not loaded" ).arg( layerPath ) );
      QLabel *msgLabel = new QLabel( tr( "%1 is an invalid layer and cannot be loaded. Please check the <a href=\"#messageLog\">message log</a> for further info." ).arg( layerPath ), messageBar() );
      msgLabel->setWordWrap( true );
      connect( msgLabel, &QLabel::linkActivated, mLogDock, &QWidget::show );
      QgsMessageBarItem *item = new QgsMessageBarItem( msgLabel, Qgis::MessageLevel::Warning );
      messageBar()->pushItem( item );
      delete layer;
    }
    //qWarning("incrementing iterator");
  }

  QgsProject::instance()->addMapLayers( myList );

  // load default style after adding to process readCustomSymbology signals
  const auto constMyList = myList;
  for ( QgsMapLayer *l : constMyList )
  {
    bool ok;
    l->loadDefaultStyle( ok );
    l->loadDefaultMetadata( ok );
  }

  QApplication::restoreOverrideCursor();
}

void QgisApp::addVirtualLayer()
{
  // show the virtual layer dialog
  QDialog *dts = dynamic_cast<QDialog *>( QgsGui::sourceSelectProviderRegistry()->createSelectionWidget( QStringLiteral( "virtual" ), this, Qt::Widget, QgsProviderRegistry::WidgetMode::Embedded ) );
  if ( !dts )
  {
    QMessageBox::warning( this, tr( "Add Virtual Layer" ), tr( "Cannot get virtual layer select dialog from provider." ) );
    return;
  }
  connect( dts, SIGNAL( addVectorLayer( QString, QString, QString ) ),
           this, SLOT( onVirtualLayerAdded( QString, QString ) ) );
  connect( dts, SIGNAL( replaceVectorLayer( QString, QString, QString, QString ) ),
           this, SLOT( replaceSelectedVectorLayer( QString, QString, QString, QString ) ) );
  dts->exec();
  delete dts;
}

void QgisApp::addSelectedVectorLayer( const QString &uri, const QString &layerName, const QString &provider )
{
  addVectorLayer( uri, layerName, provider );
}

void QgisApp::replaceSelectedVectorLayer( const QString &oldId, const QString &uri, const QString &layerName, const QString &provider )
{
  QgsMapLayer *old = QgsProject::instance()->mapLayer( oldId );
  if ( !old )
    return;
  QgsVectorLayer *oldLayer = static_cast<QgsVectorLayer *>( old );
  const QgsVectorLayer::LayerOptions options { QgsProject::instance()->transformContext() };
  QgsVectorLayer *newLayer = new QgsVectorLayer( uri, layerName, provider, options );
  if ( !newLayer || !newLayer->isValid() )
    return;

  QgsProject::instance()->addMapLayer( newLayer, /*addToLegend*/ false, /*takeOwnership*/ true );
  duplicateVectorStyle( oldLayer, newLayer );

  // insert the new layer just below the old one
  QgsLayerTreeUtils::insertLayerBelow( QgsProject::instance()->layerTreeRoot(), oldLayer, newLayer );
  // and remove the old layer
  QgsProject::instance()->removeMapLayer( oldLayer );
} // QgisApp:replaceSelectedVectorLayer

void QgisApp::fileExit()
{
  if ( QgsApplication::taskManager()->countActiveTasks() > 0 )
  {
    QStringList tasks;
    const QList< QgsTask * > activeTasks = QgsApplication::taskManager()->activeTasks();
    for ( QgsTask *task : activeTasks )
    {
      if ( task->flags() & QgsTask::CancelWithoutPrompt )
        continue;

      tasks << tr( " • %1" ).arg( task->description() );
    }

    // prompt if any tasks which require user confirmation remain, otherwise just cancel them directly and continue with shutdown.
    if ( tasks.empty() )
    {
      // all tasks can be silently terminated without warning
      QgsApplication::taskManager()->cancelAll();
    }
    else
    {
      if ( QMessageBox::question( this, tr( "Active Tasks" ),
                                  tr( "The following tasks are currently running in the background:\n\n%1\n\nDo you want to try canceling these active tasks?" ).arg( tasks.join( QLatin1Char( '\n' ) ) ),
                                  QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
      {
        QgsApplication::taskManager()->cancelAll();
      }
      return;
    }
  }

  QgsCanvasRefreshBlocker refreshBlocker;
  if ( checkUnsavedLayerEdits() && checkMemoryLayers() && saveDirty() && checkExitBlockers() )
  {
    closeProject();
    userProfileManager()->setDefaultFromActive();

    // shouldn't be needed, but from this stage on, we don't want/need ANY map canvas refreshes to take place
    mFreezeCount = 1000000;
    qApp->exit( 0 );
  }
}


bool QgisApp::fileNew()
{
  return fileNew( true ); // prompts whether to save project
} // fileNew()


bool QgisApp::fileNewBlank()
{
  return fileNew( true, true );
}

void QgisApp::fileClose()
{
  if ( fileNewBlank() )
    mCentralContainer->setCurrentIndex( 1 );
}


//as file new but accepts flags to indicate whether we should prompt to save
bool QgisApp::fileNew( bool promptToSaveFlag, bool forceBlank )
{
  if ( checkTasksDependOnProject() )
    return false;

  if ( promptToSaveFlag )
  {
    if ( !checkUnsavedLayerEdits() || !checkMemoryLayers() || !saveDirty() )
    {
      return false; //cancel pressed
    }
  }

  mProjectLastModified = QDateTime();

  QgsSettings settings;

  MAYBE_UNUSED QgsProjectDirtyBlocker dirtyBlocker( QgsProject::instance() );
  QgsCanvasRefreshBlocker refreshBlocker;
  closeProject();

  QgsProject *prj = QgsProject::instance();
  prj->layerTreeRegistryBridge()->setNewLayersVisible( settings.value( QStringLiteral( "qgis/new_layers_visible" ), true ).toBool() );

  //set the canvas to the default project background color
  mOverviewCanvas->setBackgroundColor( prj->backgroundColor() );
  applyProjectSettingsToCanvas( mMapCanvas );

  prj->setDirty( false );

  setTitleBarText_( *this );

  // emit signal so listeners know we have a new project
  emit newProject();

  mMapCanvas->clearExtentHistory();
  mMapCanvas->setRotation( 0.0 );
  mScaleWidget->updateScales();

  // set project CRS
  const QgsCoordinateReferenceSystem srs = QgsCoordinateReferenceSystem( settings.value( QStringLiteral( "/projections/defaultProjectCrs" ), geoEpsgCrsAuthId(), QgsSettings::App ).toString() );
  // write the projections _proj string_ to project settings
  const bool planimetric = settings.value( QStringLiteral( "measure/planimetric" ), true, QgsSettings::Core ).toBool();
  prj->setCrs( srs, !planimetric ); // If the default ellipsoid is not planimetric, set it from the default crs
  if ( planimetric )
    prj->setEllipsoid( geoNone() );

  /* New Empty Project Created
      (before attempting to load custom project templates/filepaths) */

  // load default template
  /* NOTE: don't open default template on launch until after initialization,
           in case a project was defined via command line */

  // don't open template if last auto-opening of a project failed
  if ( ! forceBlank )
  {
    forceBlank = ! settings.value( QStringLiteral( "qgis/projOpenedOKAtLaunch" ), QVariant( true ) ).toBool();
  }

  if ( ! forceBlank && settings.value( QStringLiteral( "qgis/newProjectDefault" ), QVariant( false ) ).toBool() )
  {
    fileNewFromDefaultTemplate();
  }

  // set the initial map tool
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::Pan ) );
  mNonEditMapTool = mMapTools->mapTool( QgsAppMapTools::Pan );  // signals are not yet setup to catch this

  prj->setDirty( false );
  return true;
}

bool QgisApp::fileNewFromTemplate( const QString &fileName )
{
  if ( checkTasksDependOnProject() )
    return false;

  if ( !checkUnsavedLayerEdits() || !checkMemoryLayers() || !saveDirty() )
  {
    return false; //cancel pressed
  }

  MAYBE_UNUSED QgsProjectDirtyBlocker dirtyBlocker( QgsProject::instance() );
  QgsDebugMsgLevel( QStringLiteral( "loading project template: %1" ).arg( fileName ), 2 );
  if ( addProject( fileName ) )
  {
    // set null filename so we don't override the template
    QgsProject::instance()->setFileName( QString() );
    return true;
  }
  return false;
}

void QgisApp::fileNewFromDefaultTemplate()
{
  QString projectTemplate = QgsApplication::qgisSettingsDirPath() + QStringLiteral( "project_default.qgs" );
  QString msgTxt;
  if ( !projectTemplate.isEmpty() && QFile::exists( projectTemplate ) )
  {
    if ( fileNewFromTemplate( projectTemplate ) )
    {
      return;
    }
    msgTxt = tr( "Default failed to open: %1" );
  }
  else
  {
    msgTxt = tr( "Default not found: %1" );
  }
  visibleMessageBar()->pushMessage( tr( "Open Template Project" ),
                                    msgTxt.arg( projectTemplate ),
                                    Qgis::MessageLevel::Warning );
}

void QgisApp::fileOpenAfterLaunch()
{
  // TODO: move auto-open project options to enums

  // check if a project is already loaded via command line or filesystem
  if ( !QgsProject::instance()->fileName().isNull() )
  {
    return;
  }

  // check if a data source is already loaded via command line or filesystem
  // empty project with layer loaded, but may not trigger a dirty project at this point
  if ( QgsProject::instance() && QgsProject::instance()->count() > 0 )
  {
    return;
  }

  // fileNewBlank() has already been called in QgisApp constructor
  // loaded project is either a new blank one, or one from command line/filesystem
  QgsSettings settings;
  QString autoOpenMsgTitle = tr( "Auto-open Project" );

  // get path of project file to open, or was attempted
  QString projPath;

  if ( mProjOpen == 0 ) // welcome page
  {
    connect( this, &QgisApp::newProject, this, &QgisApp::showMapCanvas );
    connect( this, &QgisApp::projectRead, this, &QgisApp::showMapCanvas );
    return;
  }
  if ( mProjOpen == 1 && !mRecentProjects.isEmpty() ) // most recent project
  {
    projPath = mRecentProjects.at( 0 ).path;
  }
  if ( mProjOpen == 2 ) // specific project
  {
    projPath = settings.value( QStringLiteral( "qgis/projOpenAtLaunchPath" ) ).toString();
  }

  // whether last auto-opening of a project failed
  bool projOpenedOK = settings.value( QStringLiteral( "qgis/projOpenedOKAtLaunch" ), QVariant( true ) ).toBool();

  // notify user if last attempt at auto-opening a project failed

  /* NOTE: Notification will not show if last auto-opened project failed but
      next project opened is from command line (minor issue) */

  /* TODO: Keep projOpenedOKAtLaunch from being reset to true after
      reading command line project (which happens before initialization signal) */
  if ( !projOpenedOK )
  {
    // only show the following 'auto-open project failed' message once, at launch
    settings.setValue( QStringLiteral( "qgis/projOpenedOKAtLaunch" ), QVariant( true ) );

    // set auto-open project back to 'New' to avoid re-opening bad project
    settings.setValue( QStringLiteral( "qgis/projOpenAtLaunch" ), QVariant( 0 ) );

    visibleMessageBar()->pushMessage( autoOpenMsgTitle,
                                      tr( "Failed to open: %1" ).arg( projPath ),
                                      Qgis::MessageLevel::Critical );
    return;
  }

  if ( mProjOpen == 3 ) // new project
  {
    // open default template, if defined
    if ( settings.value( QStringLiteral( "qgis/newProjectDefault" ), QVariant( false ) ).toBool() )
    {
      fileNewFromDefaultTemplate();
    }
    return;
  }

  if ( projPath.isEmpty() ) // projPath required from here
  {
    return;
  }

  // Is this a storage based project?
  const bool projectIsFromStorage { QgsApplication::projectStorageRegistry()->projectStorageFromUri( projPath ) != nullptr };

  if ( !projectIsFromStorage &&
       !projPath.endsWith( QLatin1String( ".qgs" ), Qt::CaseInsensitive ) &&
       !projPath.endsWith( QLatin1String( ".qgz" ), Qt::CaseInsensitive ) )
  {
    visibleMessageBar()->pushMessage( autoOpenMsgTitle,
                                      tr( "Not valid project file: %1" ).arg( projPath ),
                                      Qgis::MessageLevel::Warning );
    return;
  }

  if ( projectIsFromStorage || QFile::exists( projPath ) )
  {
    // set flag to check on next app launch if the following project opened OK
    settings.setValue( QStringLiteral( "qgis/projOpenedOKAtLaunch" ), QVariant( false ) );

    if ( !addProject( projPath ) )
    {
      visibleMessageBar()->pushMessage( autoOpenMsgTitle,
                                        tr( "Project failed to open: %1" ).arg( projPath ),
                                        Qgis::MessageLevel::Warning );
    }

    if ( projPath.endsWith( QLatin1String( "project_default.qgs" ) ) )
    {
      visibleMessageBar()->pushMessage( autoOpenMsgTitle,
                                        tr( "Default template has been reopened: %1" ).arg( projPath ),
                                        Qgis::MessageLevel::Info );
    }
  }
  else
  {
    visibleMessageBar()->pushMessage( autoOpenMsgTitle,
                                      tr( "File not found: %1" ).arg( projPath ),
                                      Qgis::MessageLevel::Warning );
  }
}

void QgisApp::fileOpenedOKAfterLaunch()
{
  QgsSettings settings;
  settings.setValue( QStringLiteral( "qgis/projOpenedOKAtLaunch" ), QVariant( true ) );
}

void QgisApp::fileNewFromTemplateAction( QAction *qAction )
{
  if ( ! qAction )
    return;

  if ( qAction->text() == tr( "< Blank >" ) )
  {
    fileNewBlank();
  }
  else
  {
    QgsSettings settings;
    QString templateDirName = settings.value( QStringLiteral( "qgis/projectTemplateDir" ),
                              QString( QgsApplication::qgisSettingsDirPath() + "project_templates" ) ).toString();
    fileNewFromTemplate( templateDirName + QDir::separator() + qAction->text() );
  }
}


void QgisApp::newVectorLayer()
{
  QString enc;
  QString error;
  QString fileName = QgsNewVectorLayerDialog::execAndCreateLayer( error, this, QString(), &enc, QgsProject::instance()->defaultCrsForNewLayers() );

  if ( !fileName.isEmpty() )
  {
    //then add the layer to the view
    QStringList fileNames;
    fileNames.append( fileName );
    //todo: the last parameter will change accordingly to layer type
    addVectorLayers( fileNames, enc, QStringLiteral( "file" ) );
  }
  else if ( !error.isEmpty() )
  {
    QLabel *msgLabel = new QLabel( tr( "Layer creation failed: %1" ).arg( error ), messageBar() );
    msgLabel->setWordWrap( true );
    connect( msgLabel, &QLabel::linkActivated, mLogDock, &QWidget::show );
    QgsMessageBarItem *item = new QgsMessageBarItem( msgLabel, Qgis::MessageLevel::Critical );
    messageBar()->pushItem( item );
  }
}

void QgisApp::newMemoryLayer()
{
  QgsVectorLayer *newLayer = QgsNewMemoryLayerDialog::runAndCreateLayer( this, QgsProject::instance()->defaultCrsForNewLayers() );

  if ( newLayer )
  {
    //then add the layer to the view
    QList< QgsMapLayer * > layers;
    layers << newLayer;

    QgsProject::instance()->addMapLayers( layers );
    newLayer->startEditing();
  }
}

#ifdef HAVE_SPATIALITE
void QgisApp::newSpatialiteLayer()
{
  QgsNewSpatialiteLayerDialog spatialiteDialog( this, QgsGuiUtils::ModalDialogFlags, QgsProject::instance()->defaultCrsForNewLayers() );
  spatialiteDialog.exec();
}
#endif

void QgisApp::newGeoPackageLayer()
{
  QgsNewGeoPackageLayerDialog dialog( this );
  dialog.setCrs( QgsProject::instance()->defaultCrsForNewLayers() );
  dialog.exec();
}

void QgisApp::newMeshLayer()
{
  QgsNewMeshLayerDialog dialog( this );
  dialog.setCrs( QgsProject::instance()->defaultCrsForNewLayers() );
  dialog.exec();
}

void QgisApp::newGpxLayer()
{
  QgsSettings settings;
  const QString dir = settings.value( QStringLiteral( "gps/gpxdirectory" ), QDir::homePath(), QgsSettings::App ).toString();
  QString fileName =
    QFileDialog::getSaveFileName( this,
                                  tr( "New GPX File" ),
                                  dir,
                                  tr( "GPS eXchange file" ) + " (*.gpx)" );
  if ( !fileName.isEmpty() )
  {
    fileName = QgsFileUtils::ensureFileNameHasExtension( fileName, { QStringLiteral( "gpx" )} );
    const QFileInfo fileInfo( fileName );
    settings.setValue( QStringLiteral( "gps/gpxdirectory" ), fileInfo.absolutePath(), QgsSettings::App );

    QFile outputFile( fileName );
    if ( !outputFile.open( QFile::WriteOnly | QIODevice::Truncate ) )
    {
      QMessageBox::warning( nullptr, tr( "New GPX File" ),
                            tr( "Unable to create a GPX file with the given name. "
                                "Try again with another name or in another "
                                "directory." ) );
      return;
    }

    QTextStream outStream( &outputFile );
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    outStream.setCodec( "UTF-8" );
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
    outStream << "<gpx></gpx>" << endl;
#else
    outStream << "<gpx></gpx>" << Qt::endl;
#endif
    outputFile.close();

    if ( QgsVectorLayer *trackLayer = addVectorLayer( fileName + "?type=track",
                                      fileInfo.baseName() + ", tracks", QStringLiteral( "gpx" ) ) )
      trackLayer->startEditing();
    if ( QgsVectorLayer *routeLayer = addVectorLayer( fileName + "?type=route",
                                      fileInfo.baseName() + ", routes", QStringLiteral( "gpx" ) ) )
      routeLayer->startEditing();
    if ( QgsVectorLayer *waypointLayer = addVectorLayer( fileName + "?type=waypoint",
                                         fileInfo.baseName() + ", waypoints", QStringLiteral( "gpx" ) ) )
      waypointLayer->startEditing();
  }
}

void QgisApp::showRasterCalculator()
{
  QgsRasterCalcDialog d( qobject_cast<QgsRasterLayer *>( activeLayer() ), this );
  if ( d.exec() != QDialog::Accepted )
  {
    return;
  }
  if ( d.useVirtualProvider() )
  {
    QgsRasterDataProvider::VirtualRasterParameters virtualCalcParams;
    virtualCalcParams.crs = d.outputCrs();
    virtualCalcParams.extent = d.outputRectangle();
    virtualCalcParams.width = d.numberOfColumns();
    virtualCalcParams.height = d.numberOfRows();
    virtualCalcParams.formula = d.formulaString();

    QString errorString;
    std::unique_ptr< QgsRasterCalcNode > calcNodeApp( QgsRasterCalcNode::parseRasterCalcString( d.formulaString(), errorString ) );
    if ( !calcNodeApp )
    {
      return;
    }
    QStringList rLayerDictionaryRef = calcNodeApp->cleanRasterReferences();
    QSet<QPair<QString, QString>> uniqueRasterUriTmp;

    for ( const auto &r : QgsRasterCalculatorEntry::rasterEntries() )
    {
      if ( ( ! rLayerDictionaryRef.contains( r.ref ) ) ||
           uniqueRasterUriTmp.contains( QPair( r.raster->source(), r.ref.mid( 0, r.ref.lastIndexOf( "@" ) ) ) ) ) continue;
      uniqueRasterUriTmp.insert( QPair( r.raster->source(), r.ref.mid( 0, r.ref.lastIndexOf( "@" ) ) ) );

      QgsRasterDataProvider::VirtualRasterInputLayers projectRLayer;
      projectRLayer.name = r.ref.mid( 0, r.ref.lastIndexOf( "@" ) );
      projectRLayer.provider = r.raster->dataProvider()->name();
      projectRLayer.uri = r.raster->source();

      virtualCalcParams.rInputLayers.append( projectRLayer );
    }

    addRasterLayer( QgsRasterDataProvider::encodeVirtualRasterProviderUri( virtualCalcParams ),
                    d.virtualLayerName().isEmpty() ? d.formulaString() : d.virtualLayerName(),
                    QStringLiteral( "virtualraster" ) );
  }
  else
  {
    //invoke analysis library
    QgsRasterCalculator rc( d.formulaString(),
                            d.outputFile(),
                            d.outputFormat(),
                            d.outputRectangle(),
                            d.outputCrs(),
                            d.numberOfColumns(),
                            d.numberOfRows(),
                            QgsRasterCalculatorEntry::rasterEntries(),
                            QgsProject::instance()->transformContext() );

    QProgressDialog p( tr( "Calculating raster expression…" ), tr( "Abort" ), 0, 0 );
    p.setWindowTitle( tr( "Raster calculator" ) );
    p.setWindowModality( Qt::WindowModal );
    p.setMaximum( 100.0 );
    QgsFeedback feedback;
    connect( &feedback, &QgsFeedback::progressChanged, &p, &QProgressDialog::setValue );
    connect( &p, &QProgressDialog::canceled, &feedback, &QgsFeedback::cancel );
    p.show();
    QgsRasterCalculator::Result res = rc.processCalculation( &feedback );
    switch ( res )
    {
      case QgsRasterCalculator::Success:
        if ( d.addLayerToProject() )
        {
          addRasterLayer( d.outputFile(), QFileInfo( d.outputFile() ).completeBaseName(), QStringLiteral( "gdal" ) );
        }
        visibleMessageBar()->pushMessage( tr( "Raster calculator" ),
                                          tr( "Calculation complete." ),
                                          Qgis::MessageLevel::Success );
        break;

      case QgsRasterCalculator::CreateOutputError:
        visibleMessageBar()->pushMessage( tr( "Raster calculator" ),
                                          tr( "Could not create destination file." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsRasterCalculator::InputLayerError:
        visibleMessageBar()->pushMessage( tr( "Raster calculator" ),
                                          tr( "Could not read input layer." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsRasterCalculator::Canceled:
        break;

      case QgsRasterCalculator::ParserError:
        visibleMessageBar()->pushMessage( tr( "Raster calculator" ),
                                          tr( "Could not parse raster formula." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsRasterCalculator::MemoryError:
        visibleMessageBar()->pushMessage( tr( "Raster calculator" ),
                                          tr( "Insufficient memory available for operation." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsRasterCalculator::BandError:
        visibleMessageBar()->pushMessage( tr( "Raster calculator" ),
                                          tr( "Invalid band number for input layer." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsRasterCalculator::CalculationError:
        visibleMessageBar()->pushMessage( tr( "Raster calculator" ),
                                          tr( "An error occurred while performing the calculation." ),
                                          Qgis::MessageLevel::Critical );
        break;
    }
    p.hide();
  }
}

void QgisApp::showMeshCalculator()
{
  QgsMeshLayer *meshLayer = qobject_cast<QgsMeshLayer *>( activeLayer() );
  if ( meshLayer && meshLayer->isEditable() )
  {
    QMessageBox::information( this, tr( "Mesh Calculator" ), tr( "Mesh calculator with mesh layer in edit mode is not supported." ) );
    return;
  }
  QgsMeshCalculatorDialog d( meshLayer, this );
  if ( d.exec() == QDialog::Accepted )
  {
    //invoke analysis library
    std::unique_ptr<QgsMeshCalculator> calculator = d.calculator();

    QProgressDialog p( tr( "Calculating mesh expression…" ), tr( "Abort" ), 0, 0 );
    p.setWindowModality( Qt::WindowModal );
    p.setMaximum( 100.0 );
    QgsFeedback feedback;
    connect( &feedback, &QgsFeedback::progressChanged, &p, &QProgressDialog::setValue );
    connect( &p, &QProgressDialog::canceled, &feedback, &QgsFeedback::cancel );
    p.show();
    QgsMeshCalculator::Result res = calculator->processCalculation( &feedback );
    switch ( res )
    {
      case QgsMeshCalculator::Success:
        visibleMessageBar()->pushMessage( tr( "Mesh calculator" ),
                                          tr( "Calculation complete." ),
                                          Qgis::MessageLevel::Success );
        break;

      case QgsMeshCalculator::EvaluateError:
        visibleMessageBar()->pushMessage( tr( "Mesh calculator" ),
                                          tr( "Could not evaluate the formula." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsMeshCalculator::InvalidDatasets:
        visibleMessageBar()->pushMessage( tr( "Mesh calculator" ),
                                          tr( "Invalid or incompatible datasets used." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsMeshCalculator::CreateOutputError:
        visibleMessageBar()->pushMessage( tr( "Mesh calculator" ),
                                          tr( "Could not create destination file." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsMeshCalculator::InputLayerError:
        visibleMessageBar()->pushMessage( tr( "Mesh calculator" ),
                                          tr( "Could not read input layer." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsMeshCalculator::Canceled:
        break;

      case QgsMeshCalculator::ParserError:
        visibleMessageBar()->pushMessage( tr( "Mesh calculator" ),
                                          tr( "Could not parse mesh formula." ),
                                          Qgis::MessageLevel::Critical );
        break;

      case QgsMeshCalculator::MemoryError:
        visibleMessageBar()->pushMessage( tr( "Mesh calculator" ),
                                          tr( "Insufficient memory available for operation." ),
                                          Qgis::MessageLevel::Critical );
        break;
    }
    p.hide();
  }
}


void QgisApp::showAlignRasterTool()
{
  QgsAlignRasterDialog dlg( this );
  dlg.exec();
}


void QgisApp::fileOpen()
{
  if ( checkTasksDependOnProject() )
    return;

  // possibly save any pending work before opening a new project
  if ( checkUnsavedLayerEdits() && checkMemoryLayers() && saveDirty() )
  {
    // Retrieve last used project dir from persistent settings
    QgsSettings settings;
    QString lastUsedDir = settings.value( QStringLiteral( "UI/lastProjectDir" ), QDir::homePath() ).toString();


    QStringList fileFilters;
    QStringList extensions;
    fileFilters << tr( "QGIS files" ) + QStringLiteral( " (*.qgs *.qgz *.QGS *.QGZ)" );
    extensions << QStringLiteral( "qgs" ) << QStringLiteral( "qgz" );
    for ( QgsCustomProjectOpenHandler *handler : std::as_const( mCustomProjectOpenHandlers ) )
    {
      if ( handler )
      {
        const QStringList filters = handler->filters();
        fileFilters.append( filters );
        for ( const QString &filter : filters )
          extensions.append( QgsFileUtils::extensionsFromFilter( filter ) );
      }
    }

    // generate master "all projects" extension list
    QString allEntry = tr( "All Project Files" ) + QStringLiteral( " (" );
    for ( const QString &extension : extensions )
      allEntry += QStringLiteral( "*.%1 *.%2 " ).arg( extension.toLower(), extension.toUpper() );
    allEntry.chop( 1 ); // remove trailing ' '
    allEntry += ')';
    fileFilters.insert( 0, allEntry );

    QString fullPath = QFileDialog::getOpenFileName( this,
                       tr( "Open Project" ),
                       lastUsedDir,
                       fileFilters.join( QLatin1String( ";;" ) ) );
    if ( fullPath.isNull() )
    {
      return;
    }

    QFileInfo myFI( fullPath );
    QString myPath = myFI.path();
    // Persist last used project dir
    settings.setValue( QStringLiteral( "UI/lastProjectDir" ), myPath );

    // open the selected project
    addProject( fullPath );
  }
}

void QgisApp::fileRevert()
{
  if ( QMessageBox::question( this, tr( "Revert Project" ),
                              tr( "Are you sure you want to discard all unsaved changes the current project?" ),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::No )
    return;

  if ( !checkUnsavedLayerEdits() || !checkMemoryLayers() )
    return;

  // re-open the current project
  addProject( QgsProject::instance()->fileName() );
}

void QgisApp::enableProjectMacros()
{
  mPythonMacrosEnabled = true;

  // load macros
  QgsPythonRunner::run( QStringLiteral( "qgis.utils.reloadProjectMacros()" ) );
}

bool QgisApp::addProject( const QString &projectFile )
{
  QgsCanvasRefreshBlocker refreshBlocker;

  bool returnCode = false;
  std::unique_ptr< QgsProjectDirtyBlocker > dirtyBlocker = std::make_unique< QgsProjectDirtyBlocker >( QgsProject::instance() );
  QObject connectionScope; // manually control scope of layersChanged lambda connection - we need the connection automatically destroyed when this function finishes

  bool badLayersHandled = false;
  if ( mAppBadLayersHandler )
  {
    connect( mAppBadLayersHandler, &QgsHandleBadLayersHandler::layersChanged, &connectionScope, [&badLayersHandled] { badLayersHandled = true; } );
  }

  // close the previous opened project if any
  closeProject();

  QFileInfo pfi( projectFile );
  mStatusBar->showMessage( tr( "Loading project: %1" ).arg( pfi.fileName() ) );
  qApp->processEvents();

  QApplication::setOverrideCursor( Qt::WaitCursor );

  bool autoSetupOnFirstLayer = mLayerTreeCanvasBridge->autoSetupOnFirstLayer();
  mLayerTreeCanvasBridge->setAutoSetupOnFirstLayer( false );

  // give custom handlers a chance first
  bool usedCustomHandler = false;
  bool customHandlerWantsThumbnail = false;
  QIcon customHandlerIcon;
  for ( QgsCustomProjectOpenHandler *handler : std::as_const( mCustomProjectOpenHandlers ) )
  {
    if ( handler && handler->handleProjectOpen( projectFile ) )
    {
      usedCustomHandler = true;
      customHandlerWantsThumbnail = handler->createDocumentThumbnailAfterOpen();
      customHandlerIcon = handler->icon();
      break;
    }
  }

  if ( !usedCustomHandler && !QgsProject::instance()->read( projectFile ) && !QgsZipUtils::isZipFile( projectFile ) )
  {
    QString backupFile = projectFile + "~";
    QString loadBackupPrompt;
    QMessageBox::StandardButtons buttons;
    if ( QFile( backupFile ).exists() )
    {
      loadBackupPrompt = "\n\n" + tr( "Do you want to open the backup file\n%1\ninstead?" ).arg( backupFile );
      buttons |= QMessageBox::Yes;
      buttons |= QMessageBox::No;
    }
    else
    {
      buttons |= QMessageBox::Ok;
    }
    QApplication::restoreOverrideCursor();
    mStatusBar->clearMessage();

    int r = QMessageBox::critical( this,
                                   tr( "Unable to open project" ),
                                   QgsProject::instance()->error() + loadBackupPrompt,
                                   buttons );

    if ( QMessageBox::Yes == r && addProject( backupFile ) )
    {
      // We loaded data from the backup file, but we pretend to work on the original project file.
      QgsProject::instance()->setFileName( projectFile );
      QgsProject::instance()->setDirty( true );
      mProjectLastModified = QgsProject::instance()->lastModified();
      returnCode = true;
    }
    else
    {
      returnCode = false;
    }
  }
  else
  {

    mProjectLastModified = QgsProject::instance()->lastModified();

    setTitleBarText_( *this );
    mOverviewCanvas->setBackgroundColor( QgsProject::instance()->backgroundColor() );

    applyProjectSettingsToCanvas( mMapCanvas );

    //load project scales
    bool projectScales = QgsProject::instance()->viewSettings()->useProjectScales();
    if ( projectScales )
    {
      mScaleWidget->updateScales();
    }

    mMapCanvas->updateScale();
    QgsDebugMsgLevel( QStringLiteral( "Scale restored..." ), 3 );

    mFilterLegendByMapContentAction->setChecked( QgsProject::instance()->readBoolEntry( QStringLiteral( "Legend" ), QStringLiteral( "filterByMap" ) ) );

    // Select the first layer
    if ( mLayerTreeView->layerTreeModel()->rootGroup()->findLayers().count() > 0 )
    {
      mLayerTreeView->setCurrentLayer( mLayerTreeView->layerTreeModel()->rootGroup()->findLayers().at( 0 )->layer() );
    }

    QgsSettings settings;

#ifdef WITH_BINDINGS
    // does the project have any macros?
    if ( mPythonUtils && mPythonUtils->isEnabled() )
    {
      if ( !QgsProject::instance()->readEntry( QStringLiteral( "Macros" ), QStringLiteral( "/pythonCode" ), QString() ).isEmpty() )
      {
        auto lambda = []() {QgisApp::instance()->enableProjectMacros();};
        QgsGui::pythonMacroAllowed( lambda, mInfoBar );
      }
    }
#endif

    // Check for missing layer widget dependencies
    const auto constVLayers { QgsProject::instance()->layers<QgsVectorLayer *>( ) };
    for ( QgsVectorLayer *vl : constVLayers )
    {
      if ( vl->isValid() )
      {
        resolveVectorLayerDependencies( vl );
      }
    }

    emit projectRead(); // let plug-ins know that we've read in a new
    // project so that they can check any project
    // specific plug-in state

    // add this to the list of recently used project files
    // if a custom handler was used, then we generate a thumbnail
    if ( !usedCustomHandler || !customHandlerWantsThumbnail )
      saveRecentProjectPath( false );
    else if ( !QgsProject::instance()->originalPath().isEmpty() )
    {
      // we have to delay the thumbnail creation until after the canvas has refreshed for the first time
      QMetaObject::Connection *connection = new QMetaObject::Connection();
      *connection = connect( mMapCanvas, &QgsMapCanvas::mapCanvasRefreshed, [ = ]()
      {
        QObject::disconnect( *connection );
        delete connection;
        saveRecentProjectPath( true, customHandlerIcon );
      } );
    }

    QApplication::restoreOverrideCursor();

    if ( autoSetupOnFirstLayer )
      mLayerTreeCanvasBridge->setAutoSetupOnFirstLayer( true );

    mStatusBar->showMessage( tr( "Project loaded" ), 3000 );
    returnCode = true;
  }

  if ( badLayersHandled )
  {
    dirtyBlocker.reset(); // allow project dirtying again
    QgsProject::instance()->setDirty( true );
  }

  return returnCode;
} // QgisApp::addProject(QString projectFile)



bool QgisApp::fileSave()
{
  // if we don't have a file name, then obviously we need to get one; note
  // that the project file name is reset to null in fileNew()

  if ( QgsProject::instance()->fileName().isNull() )
  {
    // Retrieve last used project dir from persistent settings
    QgsSettings settings;
    QString lastUsedDir = settings.value( QStringLiteral( "UI/lastProjectDir" ), QDir::homePath() ).toString();

    const QString qgsExt = tr( "QGIS files" ) + " (*.qgs)";
    const QString zipExt = tr( "QGZ files" ) + " (*.qgz)";

    QString exts;
    QgsProject::FileFormat defaultProjectFileFormat = settings.enumValue( QStringLiteral( "/qgis/defaultProjectFileFormat" ), QgsProject::FileFormat::Qgz );
    if ( defaultProjectFileFormat == QgsProject::FileFormat::Qgs )
    {
      exts = qgsExt + QStringLiteral( ";;" ) + zipExt;
    }
    else
    {
      exts = zipExt + QStringLiteral( ";;" ) + qgsExt;
    }
    QString filter;
    QString path = QFileDialog::getSaveFileName(
                     this,
                     tr( "Choose a QGIS project file" ),
                     lastUsedDir + '/' + QgsProject::instance()->title(),
                     exts, &filter );
    if ( path.isEmpty() )
      return false;

    QFileInfo fullPath;
    fullPath.setFile( path );

    // make sure we have the .qgs extension in the file name
    if ( filter == zipExt )
    {
      if ( fullPath.suffix().compare( QLatin1String( "qgz" ), Qt::CaseInsensitive ) != 0 )
        fullPath.setFile( fullPath.filePath() + ".qgz" );
    }
    else
    {
      if ( fullPath.suffix().compare( QLatin1String( "qgs" ), Qt::CaseInsensitive ) != 0 )
        fullPath.setFile( fullPath.filePath() + ".qgs" );
    }

    QgsProject::instance()->setFileName( fullPath.filePath() );
  }
  else
  {
    bool usingProjectStorage = QgsProject::instance()->projectStorage();
    bool fileExists = usingProjectStorage ? true : QFileInfo::exists( QgsProject::instance()->fileName() );

    if ( fileExists && !mProjectLastModified.isNull() && mProjectLastModified != QgsProject::instance()->lastModified() )
    {
      if ( QMessageBox::warning( this,
                                 tr( "Open a Project" ),
                                 tr( "The loaded project file on disk was meanwhile changed. Do you want to overwrite the changes?\n"
                                     "\nLast modification date on load was: %1"
                                     "\nCurrent last modification date is: %2" )
                                 .arg( QLocale::system().toString( mProjectLastModified, QLocale::LongFormat ),
                                       QLocale::system().toString( QgsProject::instance()->lastModified(), QLocale::LongFormat ) ),
                                 QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel )
        return false;
    }

    if ( fileExists && !usingProjectStorage && ! QFileInfo( QgsProject::instance()->fileName() ).isWritable() )
    {
      visibleMessageBar()->pushMessage( tr( "Insufficient permissions" ),
                                        tr( "The project file is not writable." ),
                                        Qgis::MessageLevel::Warning );
      return false;
    }
  }

  // Store current map view settings into the project
  QgsProject::instance()->viewSettings()->setDefaultViewExtent( QgsReferencedRectangle( mapCanvas()->extent(), QgsProject::instance()->crs() ) );

  if ( QgsProject::instance()->write() )
  {
    setTitleBarText_( *this ); // update title bar
    mStatusBar->showMessage( tr( "Saved project to: %1" ).arg( QDir::toNativeSeparators( QgsProject::instance()->fileName() ) ), 5000 );

    saveRecentProjectPath();

    mProjectLastModified = QgsProject::instance()->lastModified();
  }
  else
  {
    QMessageBox::critical( this,
                           tr( "Unable to save project %1" ).arg( QDir::toNativeSeparators( QgsProject::instance()->fileName() ) ),
                           QgsProject::instance()->error() );
    mProjectLastModified = QgsProject::instance()->lastModified();
    return false;
  }

  // run the saved project macro
  if ( mPythonMacrosEnabled )
  {
    QgsPythonRunner::run( QStringLiteral( "qgis.utils.saveProjectMacro();" ) );
  }

  return true;
} // QgisApp::fileSave

void QgisApp::fileSaveAs()
{
  QString defaultPath;
  QgsSettings settings;
  // First priority is to default to same path as existing file
  const QString currentPath = QgsProject::instance()->absoluteFilePath();
  if ( !currentPath.isEmpty() )
  {
    defaultPath = currentPath;
  }
  else
  {
    // Retrieve last used project dir from persistent settings
    defaultPath = settings.value( QStringLiteral( "UI/lastProjectDir" ), QDir::homePath() ).toString();
    defaultPath += QString( '/' + QgsProject::instance()->title() );
  }

  const QString qgsExt = tr( "QGIS files" ) + " (*.qgs *.QGS)";
  const QString zipExt = tr( "QGZ files" ) + " (*.qgz)";

  QString exts;
  QgsProject::FileFormat defaultProjectFileFormat = settings.enumValue( QStringLiteral( "/qgis/defaultProjectFileFormat" ), QgsProject::FileFormat::Qgz );
  if ( defaultProjectFileFormat == QgsProject::FileFormat::Qgs )
  {
    exts = qgsExt + QStringLiteral( ";;" ) + zipExt;
  }
  else
  {
    exts = zipExt + QStringLiteral( ";;" ) + qgsExt;
  }
  QString filter;
  QString path = QFileDialog::getSaveFileName( this,
                 tr( "Save Project As" ),
                 defaultPath,
                 exts, &filter );
  if ( path.isEmpty() )
    return;

  QFileInfo fullPath( path );

  QgsSettings().setValue( QStringLiteral( "UI/lastProjectDir" ), fullPath.path() );

  if ( filter == zipExt )
  {
    if ( fullPath.suffix().compare( QLatin1String( "qgz" ), Qt::CaseInsensitive ) != 0 )
      fullPath.setFile( fullPath.filePath() + ".qgz" );
  }
  else // .qgs
  {
    if ( fullPath.suffix().compare( QLatin1String( "qgs" ), Qt::CaseInsensitive ) != 0 )
      fullPath.setFile( fullPath.filePath() + ".qgs" );
  }

  QgsProject::instance()->setFileName( fullPath.filePath() );

  if ( QgsProject::instance()->write() )
  {
    setTitleBarText_( *this ); // update title bar
    mStatusBar->showMessage( tr( "Saved project to: %1" ).arg( QDir::toNativeSeparators( QgsProject::instance()->fileName() ) ), 5000 );
    // add this to the list of recently used project files
    saveRecentProjectPath();
  }
  else
  {
    QMessageBox::critical( this,
                           tr( "Unable to save project %1" ).arg( QDir::toNativeSeparators( QgsProject::instance()->fileName() ) ),
                           QgsProject::instance()->error(),
                           QMessageBox::Ok,
                           Qt::NoButton );
  }
  mProjectLastModified = fullPath.lastModified();
} // QgisApp::fileSaveAs

void QgisApp::dxfExport()
{
  QgsDxfExportDialog d;
  if ( d.exec() == QDialog::Accepted )
  {
    QgsDxfExport dxfExport;

    QgsMapSettings settings( mapCanvas()->mapSettings() );
    settings.setLayerStyleOverrides( QgsProject::instance()->mapThemeCollection()->mapThemeStyleOverrides( d.mapTheme() ) );
    dxfExport.setMapSettings( settings );
    dxfExport.addLayers( d.layers() );
    dxfExport.setSymbologyScale( d.symbologyScale() );
    dxfExport.setSymbologyExport( d.symbologyMode() );
    dxfExport.setLayerTitleAsName( d.layerTitleAsName() );
    dxfExport.setDestinationCrs( d.crs() );
    dxfExport.setForce2d( d.force2d() );

    QgsDxfExport::Flags flags = QgsDxfExport::Flags();
    if ( !d.useMText() )
      flags = flags | QgsDxfExport::FlagNoMText;
    dxfExport.setFlags( flags );

    if ( auto *lMapCanvas = mapCanvas() )
    {
      //extent
      if ( d.exportMapExtent() )
      {
        QgsCoordinateTransform t( lMapCanvas->mapSettings().destinationCrs(), d.crs(), QgsProject::instance() );
        t.setBallparkTransformsAreAppropriate( true );
        dxfExport.setExtent( t.transformBoundingBox( lMapCanvas->extent() ) );
      }
    }

    QString fileName( d.saveFile() );
    if ( !fileName.endsWith( QLatin1String( ".dxf" ), Qt::CaseInsensitive ) )
      fileName += QLatin1String( ".dxf" );
    QFile dxfFile( fileName );
    QApplication::setOverrideCursor( Qt::BusyCursor );
    switch ( dxfExport.writeToFile( &dxfFile, d.encoding() ) )
    {
      case QgsDxfExport::ExportResult::Success:
        visibleMessageBar()->pushMessage( tr( "DXF export completed" ), Qgis::MessageLevel::Success );
        break;

      case QgsDxfExport::ExportResult::DeviceNotWritableError:
        visibleMessageBar()->pushMessage( tr( "DXF export failed, device is not writable" ), Qgis::MessageLevel::Critical );
        break;

      case QgsDxfExport::ExportResult::InvalidDeviceError:
        visibleMessageBar()->pushMessage( tr( "DXF export failed, the device is invalid" ), Qgis::MessageLevel::Critical );
        break;

      case QgsDxfExport::ExportResult::EmptyExtentError:
        visibleMessageBar()->pushMessage( tr( "DXF export failed, the extent could not be determined" ), Qgis::MessageLevel::Critical );
        break;
    }
    QApplication::restoreOverrideCursor();
  }
}

void QgisApp::dwgImport()
{
  QgsDwgImportDialog d;
  d.exec();
}

void QgisApp::openLayerDefinition( const QString &path )
{
  QString errorMessage;
  QgsReadWriteContext context;
  bool loaded = false;

  QFile file( path );
  if ( !file.open( QIODevice::ReadOnly ) )
  {
    errorMessage = QStringLiteral( "Can not open file" );
  }
  else
  {
    QDomDocument doc;
    QString message;
    if ( !doc.setContent( &file, &message ) )
    {
      errorMessage = message;
    }
    else
    {
      QFileInfo fileinfo( file );
      QDir::setCurrent( fileinfo.absoluteDir().path() );

      context.setPathResolver( QgsPathResolver( path ) );
      context.setProjectTranslator( QgsProject::instance() );

      loaded = QgsLayerDefinition::loadLayerDefinition( doc, QgsProject::instance(), QgsProject::instance()->layerTreeRoot(), errorMessage, context );
    }
  }

  if ( loaded )
  {
    const QList< QgsReadWriteContext::ReadWriteMessage > messages = context.takeMessages();
    QVector< QgsReadWriteContext::ReadWriteMessage > shownMessages;
    for ( const QgsReadWriteContext::ReadWriteMessage &message : messages )
    {
      if ( shownMessages.contains( message ) )
        continue;

      visibleMessageBar()->pushMessage( QString(), message.message(), message.categories().join( '\n' ), message.level() );

      shownMessages.append( message );
    }
  }
  else if ( !loaded || !errorMessage.isEmpty() )
  {
    visibleMessageBar()->pushMessage( tr( "Error loading layer definition" ), errorMessage, Qgis::MessageLevel::Warning );
  }
}

void QgisApp::openTemplate( const QString &fileName )
{
  QFile templateFile;
  templateFile.setFileName( fileName );

  if ( !templateFile.open( QIODevice::ReadOnly ) )
  {
    visibleMessageBar()->pushMessage( tr( "Load template" ), tr( "Could not read template file" ), Qgis::MessageLevel::Warning );
    return;
  }

  QDomDocument templateDoc;
  if ( !templateDoc.setContent( &templateFile, false ) )
  {
    visibleMessageBar()->pushMessage( tr( "Load template" ), tr( "Could not load template file" ), Qgis::MessageLevel::Warning );
    return;
  }

  QString title;
  QDomElement layoutElem = templateDoc.documentElement();
  if ( !layoutElem.isNull() )
    title = layoutElem.attribute( QStringLiteral( "name" ) );

  if ( !uniqueLayoutTitle( this, title, true, QgsMasterLayoutInterface::PrintLayout, title ) )
  {
    return;
  }

  //create new layout object
  std::unique_ptr< QgsPrintLayout > layout = std::make_unique< QgsPrintLayout >( QgsProject::instance() );
  bool loadedOk = false;
  layout->loadFromTemplate( templateDoc, QgsReadWriteContext(), true, &loadedOk );
  if ( loadedOk )
  {
    layout->setName( title );

    openLayoutDesignerDialog( layout.get() );
    QgsProject::instance()->layoutManager()->addLayout( layout.release() );
  }
  else
  {
    visibleMessageBar()->pushMessage( tr( "Load template" ), tr( "Could not load template file" ), Qgis::MessageLevel::Warning );
  }
}

// Open the project file corresponding to the
// path at the given index in mRecentProjectPaths
void QgisApp::openProject( QAction *action )
{
  // possibly save any pending work before opening a different project
  Q_ASSERT( action );
  const QString project = action->data().toString().replace( "&&", "&" );

  if ( checkTasksDependOnProject() )
    return;

  if ( checkUnsavedLayerEdits() && checkMemoryLayers() && saveDirty() )
    addProject( project );
}

void QgisApp::runScript( const QString &filePath )
{
#ifdef WITH_BINDINGS
  if ( !mPythonUtils || !mPythonUtils->isEnabled() )
    return;

  QgsSettings settings;
  bool showScriptWarning = settings.value( QStringLiteral( "UI/showScriptWarning" ), true ).toBool();

  QMessageBox msgbox;
  if ( showScriptWarning )
  {
    msgbox.setWindowTitle( tr( "Security warning" ) );
    msgbox.setText( tr( "Executing a script from an untrusted source can harm your computer. Only continue if you trust the source of the script. Continue?" ) );
    msgbox.setIcon( QMessageBox::Icon::Warning );
    msgbox.addButton( QMessageBox::Yes );
    msgbox.addButton( QMessageBox::No );
    msgbox.setDefaultButton( QMessageBox::No );
    QCheckBox *cb = new QCheckBox( tr( "Don't show this again." ) );
    msgbox.setCheckBox( cb );
    msgbox.exec();
    settings.setValue( QStringLiteral( "UI/showScriptWarning" ), !msgbox.checkBox()->isChecked() );
  }

  if ( !showScriptWarning || msgbox.result() == QMessageBox::Yes )
  {
    mPythonUtils->runString( QString( "qgis.utils.run_script_from_file(\"%1\")" ).arg( filePath ),
                             tr( "Failed to run Python script:" ), false );
  }
#else
  Q_UNUSED( filePath )
#endif
}

void QgisApp::openProject( const QString &fileName )
{
  QgsCanvasRefreshBlocker refreshBlocker;
  if ( checkTasksDependOnProject() )
    return;

  // possibly save any pending work before opening a different project
  if ( checkUnsavedLayerEdits() && checkMemoryLayers() && saveDirty() )
  {
    // error handling and reporting is in addProject() function
    addProject( fileName );
  }
}

bool QgisApp::openLayer( const QString &fileName, bool allowInteractive )
{
  bool ok = false;
  const QFileInfo fileInfo( fileName );

  // highest priority = delegate to provider registry to handle
  const QList< QgsProviderRegistry::ProviderCandidateDetails > candidateProviders = QgsProviderRegistry::instance()->preferredProvidersForUri( fileName );
  if ( candidateProviders.size() == 1 && candidateProviders.at( 0 ).layerTypes().size() == 1 )
  {
    // one good candidate provider and possible layer type -- that makes things nice and easy!
    switch ( candidateProviders.at( 0 ).layerTypes().at( 0 ) )
    {
      case QgsMapLayerType::VectorLayer:
      case QgsMapLayerType::RasterLayer:
      case QgsMapLayerType::MeshLayer:
      case QgsMapLayerType::AnnotationLayer:
      case QgsMapLayerType::PluginLayer:
      case QgsMapLayerType::VectorTileLayer:
      case QgsMapLayerType::GroupLayer:
        // not supported here yet!
        break;

      case QgsMapLayerType::PointCloudLayer:
        ok = static_cast< bool >( addPointCloudLayerPrivate( fileName, fileInfo.completeBaseName(), candidateProviders.at( 0 ).metadata()->key(), false ) );
        break;
    }
  }

  if ( ok )
    return true;

  CPLPushErrorHandler( CPLQuietErrorHandler );

  // if needed prompt for zipitem layers
  QString vsiPrefix = QgsZipItem::vsiPrefix( fileName );
  if ( vsiPrefix == QLatin1String( "/vsizip/" ) || vsiPrefix == QLatin1String( "/vsitar/" ) )
  {
    if ( askUserForZipItemLayers( fileName, {} ) )
    {
      CPLPopErrorHandler();
      return true;
    }
  }

  if ( fileName.endsWith( QStringLiteral( ".mbtiles" ), Qt::CaseInsensitive ) )
  {
    QgsMbTiles reader( fileName );
    if ( reader.open() )
    {
      if ( reader.metadataValue( "format" ) == QLatin1String( "pbf" ) )
      {
        // these are vector tiles
        QUrlQuery uq;
        uq.addQueryItem( QStringLiteral( "type" ), QStringLiteral( "mbtiles" ) );
        uq.addQueryItem( QStringLiteral( "url" ), fileName );
        const QgsVectorTileLayer::LayerOptions options( QgsProject::instance()->transformContext() );
        std::unique_ptr<QgsVectorTileLayer> vtLayer( new QgsVectorTileLayer( uq.toString(), fileInfo.completeBaseName(), options ) );
        if ( vtLayer->isValid() )
        {
          QgsProject::instance()->addMapLayer( vtLayer.release() );
          return true;
        }
      }
      else // raster tiles
      {
        // prefer to use WMS provider's implementation to open MBTiles rasters
        QUrlQuery uq;
        uq.addQueryItem( QStringLiteral( "type" ), QStringLiteral( "mbtiles" ) );
        uq.addQueryItem( QStringLiteral( "url" ), QUrl::fromLocalFile( fileName ).toString() );
        if ( addRasterLayer( uq.toString(), fileInfo.completeBaseName(), QStringLiteral( "wms" ) ) )
          return true;
      }
    }
  }
  else if ( fileName.endsWith( QStringLiteral( ".vtpk" ), Qt::CaseInsensitive ) )
  {
    // these are vector tiles
    QUrlQuery uq;
    uq.addQueryItem( QStringLiteral( "type" ), QStringLiteral( "vtpk" ) );
    uq.addQueryItem( QStringLiteral( "url" ), fileName );
    const QgsVectorTileLayer::LayerOptions options( QgsProject::instance()->transformContext() );
    std::unique_ptr<QgsVectorTileLayer> vtLayer( new QgsVectorTileLayer( uq.toString(), fileInfo.completeBaseName(), options ) );
    if ( vtLayer->isValid() )
    {
      postProcessAddedLayer( vtLayer.get() );
      QgsProject::instance()->addMapLayer( vtLayer.release() );
      return true;
    }
  }

  QList< QgsProviderSublayerModel::NonLayerItem > nonLayerItems;
  if ( QgsProjectStorage *ps = QgsApplication::projectStorageRegistry()->projectStorageFromUri( fileName ) )
  {
    const QStringList projects = ps->listProjects( fileName );
    for ( const QString &project : projects )
    {
      QgsProviderSublayerModel::NonLayerItem projectItem;
      projectItem.setType( QStringLiteral( "project" ) );
      projectItem.setName( project );
      projectItem.setUri( QStringLiteral( "%1://%2?projectName=%3" ).arg( ps->type(), fileName, project ) );
      projectItem.setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mIconQgsProjectFile.svg" ) ) );
      nonLayerItems << projectItem;
    }
  }

  // query sublayers
  QList< QgsProviderSublayerDetails > sublayers = QgsProviderRegistry::instance()->querySublayers( fileName, Qgis::SublayerQueryFlag::IncludeSystemTables );

  if ( !sublayers.empty() || !nonLayerItems.empty() )
  {
    const bool detailsAreIncomplete = QgsProviderUtils::sublayerDetailsAreIncomplete( sublayers, QgsProviderUtils::SublayerCompletenessFlag::IgnoreUnknownFeatureCount );
    const bool singleSublayerOnly = sublayers.size() == 1;
    QString groupName;

    if ( allowInteractive && ( !singleSublayerOnly || detailsAreIncomplete || !nonLayerItems.empty() ) )
    {
      // ask user for sublayers (unless user settings dictate otherwise!)
      switch ( shouldAskUserForSublayers( sublayers, !nonLayerItems.empty() ) )
      {
        case SublayerHandling::AskUser:
        {
          // prompt user for sublayers
          QgsProviderSublayersDialog dlg( fileName, fileName, sublayers, {}, this );
          dlg.setNonLayerItems( nonLayerItems );

          if ( dlg.exec() )
          {
            sublayers = dlg.selectedLayers();
            nonLayerItems = dlg.selectedNonLayerItems();
          }
          else
          {
            sublayers.clear(); // dialog was canceled, so don't add any sublayers
            nonLayerItems.clear();
          }
          groupName = dlg.groupName();
          break;
        }

        case SublayerHandling::LoadAll:
        {
          if ( detailsAreIncomplete )
          {
            // requery sublayers, resolving geometry types
            sublayers = QgsProviderRegistry::instance()->querySublayers( fileName, Qgis::SublayerQueryFlag::ResolveGeometryType );
          }
          break;
        }

        case SublayerHandling::AbortLoading:
          sublayers.clear(); // don't add any sublayers
          break;
      };
    }
    else if ( detailsAreIncomplete )
    {
      // requery sublayers, resolving geometry types
      sublayers = QgsProviderRegistry::instance()->querySublayers( fileName, Qgis::SublayerQueryFlag::ResolveGeometryType );
    }

    ok = true;

    // now add sublayers
    if ( !sublayers.empty() )
    {
      QgsCanvasRefreshBlocker refreshBlocker;
      QgsSettings settings;

      QString base = QgsProviderUtils::suggestLayerNameFromFilePath( fileName );
      if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
      {
        base = QgsMapLayer::formatLayerName( base );
      }

      addSublayers( sublayers, base, groupName );
      activateDeactivateLayerRelatedActions( activeLayer() );
    }
    else if ( !nonLayerItems.empty() )
    {
      QgsCanvasRefreshBlocker refreshBlocker;
      if ( checkTasksDependOnProject() )
        return true;

      // possibly save any pending work before opening a different project
      if ( checkUnsavedLayerEdits() && checkMemoryLayers() && saveDirty() )
      {
        // error handling and reporting is in addProject() function
        addProject( nonLayerItems.at( 0 ).uri() );
      }
      return true;
    }
  }

  CPLPopErrorHandler();

  if ( !ok )
  {
    // maybe a known file type, which couldn't be opened due to a missing dependency... (eg. las for a non-pdal-enabled build)
    QgsProviderRegistry::UnusableUriDetails details;
    if ( QgsProviderRegistry::instance()->handleUnusableUri( fileName, details ) )
    {
      ok = true;

      if ( details.detailedWarning.isEmpty() )
        visibleMessageBar()->pushMessage( QString(), details.warning, Qgis::MessageLevel::Critical );
      else
        visibleMessageBar()->pushMessage( QString(), details.warning, details.detailedWarning, Qgis::MessageLevel::Critical );
    }
  }

  if ( !ok )
  {
    // we have no idea what this file is...
    QgsMessageLog::logMessage( tr( "Unable to load %1" ).arg( fileName ) );

    const QString msg = tr( "%1 is not a valid or recognized data source." ).arg( fileName );
    visibleMessageBar()->pushMessage( tr( "Invalid Data Source" ), msg, Qgis::MessageLevel::Critical );
  }

  return ok;
}


// Open a file specified by a commandline argument, Drop or FileOpen event.
void QgisApp::openFile( const QString &fileName, const QString &fileTypeHint )
{
  // check to see if we are opening a project file
  QFileInfo fi( fileName );
  if ( fileTypeHint == QLatin1String( "project" ) || fi.suffix().compare( QLatin1String( "qgs" ), Qt::CaseInsensitive ) == 0 || fi.suffix().compare( QLatin1String( "qgz" ), Qt::CaseInsensitive ) == 0 )
  {
    QgsDebugMsgLevel( "Opening project " + fileName, 2 );
    openProject( fileName );
  }
  else if ( fi.suffix().compare( QLatin1String( "qlr" ), Qt::CaseInsensitive ) == 0 )
  {
    openLayerDefinition( fileName );
  }
  else if ( fi.suffix().compare( QLatin1String( "qpt" ), Qt::CaseInsensitive ) == 0 )
  {
    openTemplate( fileName );
  }
  else if ( fi.suffix().compare( QLatin1String( "py" ), Qt::CaseInsensitive ) == 0 )
  {
    runScript( fileName );
  }
  else
  {
    QgsDebugMsgLevel( "Adding " + fileName + " to the map canvas", 2 );
    openLayer( fileName, true );
  }
}

void QgisApp::newPrintLayout()
{
  QString title;
  if ( !uniqueLayoutTitle( this, title, true, QgsMasterLayoutInterface::PrintLayout ) )
  {
    return;
  }
  createNewPrintLayout( title );
}

void QgisApp::newReport()
{
  QString title;
  if ( !uniqueLayoutTitle( this, title, true, QgsMasterLayoutInterface::Report ) )
  {
    return;
  }
  createNewReport( title );
}

void QgisApp::disablePreviewMode()
{
  mMapCanvas->setPreviewModeEnabled( false );
}

void QgisApp::activateMonoPreview()
{
  mMapCanvas->setPreviewModeEnabled( true );
  mMapCanvas->setPreviewMode( QgsPreviewEffect::PreviewMono );
}

void QgisApp::activateGrayscalePreview()
{
  mMapCanvas->setPreviewModeEnabled( true );
  mMapCanvas->setPreviewMode( QgsPreviewEffect::PreviewGrayscale );
}

void QgisApp::activateProtanopePreview()
{
  mMapCanvas->setPreviewModeEnabled( true );
  mMapCanvas->setPreviewMode( QgsPreviewEffect::PreviewProtanope );
}

void QgisApp::activateDeuteranopePreview()
{
  mMapCanvas->setPreviewModeEnabled( true );
  mMapCanvas->setPreviewMode( QgsPreviewEffect::PreviewDeuteranope );
}

void QgisApp::activateTritanopePreview()
{
  mMapCanvas->setPreviewModeEnabled( true );
  mMapCanvas->setPreviewMode( QgsPreviewEffect::PreviewTritanope );
}

void QgisApp::toggleFilterLegendByExpression( bool checked )
{
  QgsLayerTreeNode *node = mLayerTreeView->currentNode();
  if ( ! node )
    return;

  if ( QgsLayerTree::isLayer( node ) )
  {
    QString e = mLegendExpressionFilterButton->expressionText();
    QgsLayerTreeUtils::setLegendFilterByExpression( *QgsLayerTree::toLayer( node ), e, checked );
  }

  updateFilterLegend();
}

void QgisApp::updateFilterLegend()
{
  bool hasExpressions = mLegendExpressionFilterButton->isChecked() && QgsLayerTreeUtils::hasLegendFilterExpression( *mLayerTreeView->layerTreeModel()->rootGroup() );
  if ( mFilterLegendByMapContentAction->isChecked() || hasExpressions )
  {
    layerTreeView()->layerTreeModel()->setLegendFilter( &mMapCanvas->mapSettings(),
        /* useExtent */ mFilterLegendByMapContentAction->isChecked(),
        /* polygon */ QgsGeometry(),
        hasExpressions );
  }
  else
  {
    layerTreeView()->layerTreeModel()->setLegendFilterByMap( nullptr );
  }
}

QList< QgsMapDecoration * > QgisApp::activeDecorations()
{
  QList< QgsMapDecoration * > decorations;
  const auto constMDecorationItems = mDecorationItems;
  for ( QgsDecorationItem *decoration : constMDecorationItems )
  {
    if ( decoration->enabled() )
    {
      decorations << decoration;
    }
  }
  return decorations;
}
void QgisApp::saveMapAsImage()
{
  QgsMapSaveDialog *dlg = new QgsMapSaveDialog( this, mMapCanvas, activeDecorations(), QgsProject::instance()->annotationManager()->annotations() );
  dlg->setAttribute( Qt::WA_DeleteOnClose );
  dlg->show();
} // saveMapAsImage

void QgisApp::saveMapAsPdf()
{
  QgsMapSaveDialog *dlg = new QgsMapSaveDialog( this, mMapCanvas, activeDecorations(), QgsProject::instance()->annotationManager()->annotations(), QgsMapSaveDialog::Pdf );
  dlg->setAttribute( Qt::WA_DeleteOnClose );
  dlg->show();
} // saveMapAsPdf

//overloaded version of the above function
void QgisApp::saveMapAsImage( const QString &imageFileNameQString, QPixmap *theQPixmap )
{
  if ( imageFileNameQString.isEmpty() )
  {
    //no fileName chosen
    return;
  }
  else
  {
    //force the size of the canvas
    mMapCanvas->resize( theQPixmap->width(), theQPixmap->height() );
    //save the mapview to the selected file
    mMapCanvas->saveAsImage( imageFileNameQString, theQPixmap );
  }
} // saveMapAsImage

//reimplements method from base (gui) class
void QgisApp::addAllToOverview()
{
  if ( mLayerTreeView )
  {
    const auto constFindLayers = mLayerTreeView->layerTreeModel()->rootGroup()->findLayers();
    for ( QgsLayerTreeLayer *nodeL : constFindLayers )
      nodeL->setCustomProperty( QStringLiteral( "overview" ), 1 );
  }

  markDirty();
}

//reimplements method from base (gui) class
void QgisApp::removeAllFromOverview()
{
  if ( mLayerTreeView )
  {
    const auto constFindLayers = mLayerTreeView->layerTreeModel()->rootGroup()->findLayers();
    for ( QgsLayerTreeLayer *nodeL : constFindLayers )
      nodeL->setCustomProperty( QStringLiteral( "overview" ), 0 );
  }

  markDirty();
}

void QgisApp::toggleFullScreen()
{
  QgsCanvasRefreshBlocker refreshBlocker;
  if ( mFullScreenMode )
  {
    if ( mPrevScreenModeMaximized )
    {
      // Change to maximized state. Just calling showMaximized() results in
      // the window going to the normal state. Calling showNormal() then
      // showMaxmized() is a work-around. Turn off rendering for this as it
      // would otherwise cause two re-renders of the map, which can take a
      // long time.
      showNormal();
      showMaximized();
      mPrevScreenModeMaximized = false;
    }
    else
    {
      showNormal();
    }
    mFullScreenMode = false;
  }
  else
  {
    if ( isMaximized() )
    {
      mPrevScreenModeMaximized = true;
    }
    showFullScreen();
    mFullScreenMode = true;
  }
}

void QgisApp::togglePanelsVisibility()
{
  toggleReducedView( false );
}

void QgisApp::toggleMapOnly()
{
  toggleReducedView( true );
}

void QgisApp::toggleReducedView( bool viewMapOnly )
{
  QgsSettings settings;

  QStringList docksTitle = settings.value( QStringLiteral( "UI/hiddenDocksTitle" ), QStringList() ).toStringList();
  QStringList docksActive = settings.value( QStringLiteral( "UI/hiddenDocksActive" ), QStringList() ).toStringList();
  QStringList toolBarsActive = settings.value( QStringLiteral( "UI/hiddenToolBarsActive" ), QStringList() ).toStringList();

  const QList<QDockWidget *> docks = findChildren<QDockWidget *>();
  const QList<QTabBar *> tabBars = findChildren<QTabBar *>();
  const QList<QToolBar *> toolBars = findChildren<QToolBar *>();

  bool allWidgetsVisible = settings.value( QStringLiteral( "UI/allWidgetsVisible" ), true ).toBool();

  if ( allWidgetsVisible )  // that is: currently nothing is hidden
  {

    if ( viewMapOnly )  //
    {
      // hide also statusbar and menubar and all toolbars
      for ( QToolBar *toolBar : toolBars )
      {
        if ( toolBar->isVisible() && !toolBar->isFloating() && toolBar->parent()->inherits( "QMainWindow" ) )
        {
          // remember the active toolbars
          toolBarsActive << toolBar->windowTitle();
          toolBar->setVisible( false );
        }
      }

      this->menuBar()->setVisible( false );
      this->statusBar()->setVisible( false );

      settings.setValue( QStringLiteral( "UI/hiddenToolBarsActive" ), toolBarsActive );
    }

    for ( QDockWidget *dock : docks )
    {
      if ( dock->isVisible() && dockWidgetArea( dock ) != Qt::NoDockWidgetArea )
      {
        // remember the active docs
        docksTitle << dock->windowTitle();
        dock->setVisible( false );
      }
    }

    docksActive.reserve( tabBars.size() );
    for ( QTabBar *tabBar : tabBars )
    {
      // remember the active tab from the docks
      docksActive << tabBar->tabText( tabBar->currentIndex() );
    }

    settings.setValue( QStringLiteral( "UI/hiddenDocksTitle" ), docksTitle );
    settings.setValue( QStringLiteral( "UI/hiddenDocksActive" ), docksActive );

    settings.setValue( QStringLiteral( "UI/allWidgetsVisible" ), false );
  }
  else  // currently panels or other widgets are hidden: show ALL based on 'remembered UI settings'
  {
    for ( QDockWidget *dock : docks )
    {
      if ( docksTitle.contains( dock->windowTitle() ) )
      {
        dock->setVisible( true );
      }
    }

    for ( QTabBar *tabBar : tabBars )
    {
      for ( int i = 0; i < tabBar->count(); ++i )
      {
        if ( docksActive.contains( tabBar->tabText( i ) ) )
        {
          tabBar->setCurrentIndex( i );
        }
      }
    }

    for ( QToolBar *toolBar : toolBars )
    {
      if ( toolBarsActive.contains( toolBar->windowTitle() ) )
      {
        toolBar->setVisible( true );
      }
    }
    this->menuBar()->setVisible( true );
    this->statusBar()->setVisible( true );

    settings.remove( QStringLiteral( "UI/hiddenToolBarsActive" ) );
    settings.remove( QStringLiteral( "UI/hiddenDocksTitle" ) );
    settings.remove( QStringLiteral( "UI/hiddenDocksActive" ) );

    settings.setValue( QStringLiteral( "UI/allWidgetsVisible" ), true );
  }
}

void QgisApp::showActiveWindowMinimized()
{
  QWidget *window = QApplication::activeWindow();
  if ( window )
  {
    window->showMinimized();
  }
}

void QgisApp::toggleActiveWindowMaximized()
{
  QWidget *window = QApplication::activeWindow();
  if ( window )
  {
    if ( window->isMaximized() )
      window->showNormal();
    else
      window->showMaximized();
  }
}

void QgisApp::activate()
{
  raise();
  setWindowState( windowState() & ~Qt::WindowMinimized );
  activateWindow();
}

void QgisApp::bringAllToFront()
{
  QgsGui::nativePlatformInterface()->currentAppActivateIgnoringOtherApps();
}

void QgisApp::addWindow( QAction *action )
{
#ifdef Q_OS_MAC
  mWindowActions->addAction( action );
  //  mWindowMenu->addAction( action ); // SG add
  action->setCheckable( true );
  action->setChecked( true );
#else
  Q_UNUSED( action )
#endif
}

void QgisApp::removeWindow( QAction *action )
{
#ifdef Q_OS_MAC
  mWindowActions->removeAction( action );
  // mWindowMenu->removeAction( action ); // SG add
#else
  Q_UNUSED( action )
#endif
}

void QgisApp::stopRendering()
{
  const auto canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
    canvas->stopRendering();
}

void QgisApp::hideAllLayers()
{
  QgsDebugMsgLevel( QStringLiteral( "hiding all layers!" ), 3 );

  const auto constChildren = mLayerTreeView->layerTreeModel()->rootGroup()->children();
  for ( QgsLayerTreeNode *node : constChildren )
  {
    node->setItemVisibilityCheckedRecursive( false );
  }
}

void QgisApp::showAllLayers()
{
  QgsDebugMsgLevel( QStringLiteral( "Showing all layers!" ), 3 );
  mLayerTreeView->layerTreeModel()->rootGroup()->setItemVisibilityCheckedRecursive( true );
}

void QgisApp::hideSelectedLayers()
{
  QgsDebugMsgLevel( QStringLiteral( "hiding selected layers!" ), 3 );

  const auto constSelectedNodes = mLayerTreeView->selectedNodes();
  for ( QgsLayerTreeNode *node : constSelectedNodes )
  {
    node->setItemVisibilityChecked( false );
  }
}

void QgisApp::toggleSelectedLayers()
{
  QgsDebugMsgLevel( QStringLiteral( "toggling selected layers!" ), 3 );

  const auto constSelectedNodes = mLayerTreeView->selectedNodes();
  if ( ! constSelectedNodes.isEmpty() )
  {
    bool isFirstNodeChecked = constSelectedNodes[0]->itemVisibilityChecked();
    for ( QgsLayerTreeNode *node : constSelectedNodes )
    {
      node->setItemVisibilityChecked( ! isFirstNodeChecked );
    }
  }
}

void QgisApp::toggleSelectedLayersIndependently()
{
  QgsDebugMsgLevel( QStringLiteral( "toggling selected layers independently!" ), 3 );

  const auto constSelectedNodes = mLayerTreeView->selectedNodes();
  if ( ! constSelectedNodes.isEmpty() )
  {
    for ( QgsLayerTreeNode *node : constSelectedNodes )
    {
      node->setItemVisibilityChecked( ! node->itemVisibilityChecked() );
    }
  }
}

void QgisApp::hideDeselectedLayers()
{
  QList<QgsLayerTreeLayer *> selectedLayerNodes = mLayerTreeView->selectedLayerNodes();

  const auto constFindLayers = mLayerTreeView->layerTreeModel()->rootGroup()->findLayers();
  for ( QgsLayerTreeLayer *nodeLayer : constFindLayers )
  {
    if ( selectedLayerNodes.contains( nodeLayer ) )
      continue;
    nodeLayer->setItemVisibilityChecked( false );
  }
}

void QgisApp::showSelectedLayers()
{
  QgsDebugMsgLevel( QStringLiteral( "show selected layers!" ), 3 );

  const auto constSelectedNodes = mLayerTreeView->selectedNodes();
  for ( QgsLayerTreeNode *node : constSelectedNodes )
  {
    QgsLayerTreeNode *nodeIter = node;
    while ( nodeIter )
    {
      nodeIter->setItemVisibilityChecked( true );
      nodeIter = nodeIter->parent();
    }
  }
}


void QgisApp::zoomIn()
{
  QgsDebugMsgLevel( QStringLiteral( "Setting map tool to zoomIn" ), 2 );

  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::ZoomIn ) );
}


void QgisApp::zoomOut()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::ZoomOut ) );
}

void QgisApp::zoomToSelected()
{
  const QList<QgsMapLayer *> layers = mLayerTreeView->selectedLayers();

  if ( layers.size() > 1 )
    mMapCanvas->zoomToSelected( layers );

  else
    mMapCanvas->zoomToSelected();

}

void QgisApp::panToSelected()
{
  const QList<QgsMapLayer *> layers = mLayerTreeView->selectedLayers();

  if ( layers.size() > 1 )
    mMapCanvas->panToSelected( layers );
  else
    mMapCanvas->panToSelected();
}

void QgisApp::pan()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::Pan ) );
}

void QgisApp::zoomFull()
{
  mMapCanvas->zoomToProjectExtent();
}

void QgisApp::zoomToPrevious()
{
  mMapCanvas->zoomToPreviousExtent();
}

void QgisApp::zoomToNext()
{
  mMapCanvas->zoomToNextExtent();
}

void QgisApp::zoomActualSize()
{
  legendLayerZoomNative();
}

void QgisApp::identify()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::Identify ) );
}

void QgisApp::doFeatureAction()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::FeatureAction ) );
}

void QgisApp::updateDefaultFeatureAction( QAction *action )
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( activeLayer() );
  if ( !vlayer )
    return;

  mActionFeatureAction->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mAction.svg" ) ) );
  mActionFeatureAction->setToolTip( tr( "No action selected" ) );

  mFeatureActionMenu->setActiveAction( action );

  QgsAction qgsAction;
  if ( action )
  {
    qgsAction = action->data().value<QgsAction>();
  }

  if ( qgsAction.isValid() )
  {
    vlayer->actions()->setDefaultAction( QStringLiteral( "Canvas" ), qgsAction.id() );
    QgsGui::mapLayerActionRegistry()->setDefaultActionForLayer( vlayer, nullptr );

    mActionFeatureAction->setToolTip( tr( "Run feature action<br><b>%1</b>" ).arg( qgsAction.name() ) );

    if ( !qgsAction.icon().isNull() )
      mActionFeatureAction->setIcon( qgsAction.icon() );
    else
      mActionFeatureAction->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionActive.svg" ) ) );
  }
  else
  {
    //action is from QgsMapLayerActionRegistry
    vlayer->actions()->setDefaultAction( QStringLiteral( "Canvas" ), QString() );

    QgsMapLayerAction *mapLayerAction = qobject_cast<QgsMapLayerAction *>( action );
    if ( mapLayerAction )
    {
      QgsGui::mapLayerActionRegistry()->setDefaultActionForLayer( vlayer, mapLayerAction );

      if ( !mapLayerAction->text().isEmpty() )
        mActionFeatureAction->setToolTip( tr( "Run feature action<br><b>%1</b>" ).arg( mapLayerAction->text() ) );

      if ( !mapLayerAction->icon().isNull() )
        mActionFeatureAction->setIcon( mapLayerAction->icon() );
      else
        mActionFeatureAction->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionActive.svg" ) ) );
    }
    else
    {
      QgsGui::mapLayerActionRegistry()->setDefaultActionForLayer( vlayer, nullptr );
    }
  }
}

void QgisApp::refreshFeatureActions()
{
  mFeatureActionMenu->clear();

  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( activeLayer() );
  if ( !vlayer )
    return;

  QList<QgsAction> actions = vlayer->actions()->actions( QStringLiteral( "Canvas" ) );
  const auto constActions = actions;
  for ( const QgsAction &action : constActions )
  {
    if ( !vlayer->isEditable() && action.isEnabledOnlyWhenEditable() )
      continue;

    QString actionTitle = !action.shortTitle().isEmpty() ? action.shortTitle() : action.icon().isNull() ? action.name() : QString();
    QAction *qAction = new QAction( action.icon(), actionTitle, mFeatureActionMenu );
    qAction->setData( QVariant::fromValue<QgsAction>( action ) );
    mFeatureActionMenu->addAction( qAction );

    if ( action.name() == vlayer->actions()->defaultAction( QStringLiteral( "Canvas" ) ).name() )
    {
      mFeatureActionMenu->setActiveAction( qAction );
    }
  }

  //add actions registered in QgsMapLayerActionRegistry
  QList<QgsMapLayerAction *> registeredActions = QgsGui::mapLayerActionRegistry()->mapLayerActions( vlayer, QgsMapLayerAction::SingleFeature );
  if ( !actions.isEmpty() && !registeredActions.empty() )
  {
    //add a separator between user defined and standard actions
    mFeatureActionMenu->addSeparator();
  }

  for ( int i = 0; i < registeredActions.size(); i++ )
  {
    mFeatureActionMenu->addAction( registeredActions.at( i ) );
    if ( registeredActions.at( i ) == QgsGui::mapLayerActionRegistry()->defaultActionForLayer( vlayer ) )
    {
      mFeatureActionMenu->setActiveAction( registeredActions.at( i ) );
    }
  }

  updateDefaultFeatureAction( mFeatureActionMenu->activeAction() );
}

void QgisApp::changeDataSource( QgsMapLayer *layer )
{
  QgsMapLayerType layerType( layer->type() );

  QgsDataSourceSelectDialog dlg( mBrowserModel, true, layerType );
  if ( !layer->isValid() )
    dlg.setWindowTitle( tr( "Repair Data Source" ) );

  const QVariantMap sourceParts = QgsProviderRegistry::instance()->decodeUri( layer->providerType(), layer->publicSource() );
  QString source = layer->publicSource();
  if ( sourceParts.contains( QStringLiteral( "path" ) ) )
  {
    const QString path = sourceParts.value( QStringLiteral( "path" ) ).toString();
    const QString closestPath = QFile::exists( path ) ? path : QgsFileUtils::findClosestExistingPath( path );
    source.replace( path, QStringLiteral( "<a href=\"%1\">%2</a>" ).arg( QUrl::fromLocalFile( closestPath ).toString(),
                    path ) );
  }
  dlg.setDescription( tr( "Original source URI: %1" ).arg( source ) );

  const QVariantMap originalSourceParts = QgsProviderRegistry::instance()->decodeUri( layer->providerType(), layer->source() );

  if ( dlg.exec() == QDialog::Accepted )
  {
    QgsMimeDataUtils::Uri uri( dlg.uri() );
    if ( uri.isValid() )
    {
      auto fixLayer = [this]( QgsMapLayer * layer, const QgsMimeDataUtils::Uri & uri )
      {
        bool layerWasValid( layer->isValid() );
        const QString previousProvider = layer->providerType();
        // Store subset string from vlayer if we are fixing a bad layer
        QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
        QString subsetString;
        // Get the subset string directly from the data provider because
        // layer's method will return a null string from invalid layers
        if ( vlayer && vlayer->dataProvider() &&
             vlayer->dataProvider()->supportsSubsetString() &&
             !vlayer->dataProvider()->subsetString( ).isEmpty() )
        {
          subsetString = vlayer->dataProvider()->subsetString();
        }
        if ( vlayer && subsetString.isEmpty() )
        {
          // actually -- the above isn't true in all situations. If a layer was invalid at the time
          // that the subset string was set, then ONLY the layer has knowledge of this subset string!
          subsetString = vlayer->subsetString();
        }

        QString newProvider = uri.providerKey;
        QString newUri = uri.uri;
        // special case -- if layer was using delimitedtext provider, and a new CSV file is picked, we shouldn't change the
        // provider to OGR
        if ( previousProvider.compare( QLatin1String( "delimitedtext" ), Qt::CaseInsensitive ) == 0
             && newProvider.compare( QLatin1String( "ogr" ), Qt::CaseInsensitive ) == 0 )
        {
          QVariantMap uriParts = QgsProviderRegistry::instance()->decodeUri( layer->providerType(), layer->source() );
          const QVariantMap newUriParts = QgsProviderRegistry::instance()->decodeUri( uri.providerKey, uri.uri );
          const QString newPath = newUriParts.value( QStringLiteral( "path" ) ).toString();
          if ( QFileInfo( newPath ).suffix().compare( QLatin1String( "csv" ), Qt::CaseInsensitive ) == 0 )
          {
            newProvider = QStringLiteral( "delimitedtext" );
            // keep all the other delimited text settings, such as field names etc, just change the path
            uriParts.insert( QStringLiteral( "path" ), newPath );
            newUri = QgsProviderRegistry::instance()->encodeUri( newProvider, uriParts );
          }
        }

        layer->setDataSource( newUri, layer->name(), newProvider, QgsDataProvider::ProviderOptions() );
        // Re-apply original style and subset string  when fixing bad layers
        if ( !( layerWasValid || layer->originalXmlProperties().isEmpty() ) )
        {
          if ( ! subsetString.isEmpty() )
          {
            vlayer->setSubsetString( subsetString );
          }
          QgsReadWriteContext context;
          context.setPathResolver( QgsProject::instance()->pathResolver() );
          context.setProjectTranslator( QgsProject::instance() );
          QString errorMsg;
          QDomDocument doc;
          if ( doc.setContent( layer->originalXmlProperties() ) )
          {
            QDomNode layer_node( doc.firstChild( ) );
            if ( ! layer->readSymbology( layer_node, errorMsg, context ) )
            {
              QgsDebugMsg( QStringLiteral( "Failed to restore original layer style from stored XML for layer %1: %2" )
                           .arg( layer->name( ) )
                           .arg( errorMsg ) );
            }
          }
          else
          {
            QgsDebugMsg( QStringLiteral( "Failed to create XML QDomDocument for layer %1: %2" )
                         .arg( layer->name( ) )
                         .arg( errorMsg ) );
          }
        }
        else if ( !subsetString.isEmpty() )
        {
          vlayer->setSubsetString( subsetString );
        }

        if ( vlayer )
          vlayer->updateExtents();

        // All the following code is necessary to refresh the layer
        QgsLayerTreeModel *model = qobject_cast<QgsLayerTreeModel *>( mLayerTreeView->model() );
        if ( model )
        {
          QgsLayerTreeLayer *tl( model->rootGroup()->findLayer( layer->id() ) );
          if ( tl && tl->itemVisibilityChecked() )
          {
            tl->setItemVisibilityChecked( false );
            tl->setItemVisibilityChecked( true );
          }
        }

        // Tell the bridge that we have fixed a layer
        if ( ! layerWasValid && layer->isValid() )
        {
          QgsProject::instance()->layerTreeRoot()->customLayerOrderChanged( );
        }
      };

      fixLayer( layer, uri );
      const QVariantMap fixedUriParts = QgsProviderRegistry::instance()->decodeUri( layer->providerType(), layer->source() );

      // next, we loop through to see if we can auto-fix any other layers with the same source
      if ( originalSourceParts.contains( QStringLiteral( "path" ) ) )
      {
        const QString originalPath = originalSourceParts.value( QStringLiteral( "path" ) ).toString();
        const QFileInfo originalPathFi( originalPath );

        const QMap< QString, QgsMapLayer * > layers = QgsProject::instance()->mapLayers( false );
        for ( auto it = layers.begin(); it != layers.end(); ++it )
        {
          if ( it.value()->isValid() )
            continue;

          QVariantMap thisParts = QgsProviderRegistry::instance()->decodeUri( it.value()->providerType(), it.value()->source() );
          if ( thisParts.contains( QStringLiteral( "path" ) ) )
          {
            const QString thisBrokenPath = thisParts.value( QStringLiteral( "path" ) ).toString();
            QString fixedPath;

            const QFileInfo thisBrokenPathFi( thisBrokenPath );
            if ( thisBrokenPath == originalPath )
            {
              // found a broken layer with the same original path, fix this one too
              fixedPath = fixedUriParts.value( QStringLiteral( "path" ) ).toString();
            }
            else if ( thisBrokenPathFi.path() == originalPathFi.path() )
            {
              // file from same original directory
              QDir fixedDir = QFileInfo( fixedUriParts.value( QStringLiteral( "path" ) ).toString() ).dir();
              const QString newCandidatePath = fixedDir.filePath( thisBrokenPathFi.fileName() );
              if ( QFileInfo::exists( newCandidatePath ) )
                fixedPath = newCandidatePath;
            }

            if ( !fixedPath.isEmpty() )
            {
              uri.uri = it.value()->source().replace( thisBrokenPath, fixedPath );
              uri.providerKey = it.value()->providerType();
              fixLayer( it.value(), uri );
            }
          }
        }
      }
    }
  }
}

void QgisApp::measure()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::MeasureDistance ) );
}

void QgisApp::measureArea()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::MeasureArea ) );
}

void QgisApp::measureAngle()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::MeasureAngle ) );
}

void QgisApp::addFormAnnotation()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::FormAnnotation ) );
}

void QgisApp::addHtmlAnnotation()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::HtmlAnnotation ) );
}

void QgisApp::addTextAnnotation()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::TextAnnotation ) );
}

void QgisApp::addSvgAnnotation()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::SvgAnnotation ) );
}

void QgisApp::modifyAnnotation()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::Annotation ) );
}

void QgisApp::reprojectAnnotations()
{
  const auto annotations = annotationItems();
  for ( QgsMapCanvasAnnotationItem *annotation : annotations )
  {
    annotation->updatePosition();
  }
}

void QgisApp::labelingFontNotFound( QgsVectorLayer *vlayer, const QString &fontfamily )
{
  // TODO: update when pref for how to resolve missing family (use matching algorithm or just default font) is implemented
  QString substitute = tr( "Default system font substituted." );

  QToolButton *btnOpenPrefs = new QToolButton();
  btnOpenPrefs->setStyleSheet( QStringLiteral( "QToolButton{ background-color: rgba(255, 255, 255, 0); color: black; text-decoration: underline; }" ) );
  btnOpenPrefs->setCursor( Qt::PointingHandCursor );
  btnOpenPrefs->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );
  btnOpenPrefs->setToolButtonStyle( Qt::ToolButtonTextOnly );

  // store pointer to vlayer in data of QAction
  QAction *act = new QAction( btnOpenPrefs );
  act->setData( QVariant( QMetaType::QObjectStar, &vlayer ) );
  act->setText( tr( "Open labeling dialog" ) );
  btnOpenPrefs->addAction( act );
  btnOpenPrefs->setDefaultAction( act );
  btnOpenPrefs->setToolTip( QString() );
  connect( btnOpenPrefs, &QToolButton::triggered, this, &QgisApp::labelingDialogFontNotFound );

  // no timeout set, since notice needs attention and is only shown first time layer is labeled
  QgsMessageBarItem *fontMsg = new QgsMessageBarItem(
    tr( "Labeling" ),
    tr( "Font for layer <b><u>%1</u></b> was not found (<i>%2</i>). %3" ).arg( vlayer->name(), fontfamily, substitute ),
    btnOpenPrefs,
    Qgis::MessageLevel::Warning,
    0,
    messageBar() );
  messageBar()->pushItem( fontMsg );
}

void QgisApp::commitError( QgsVectorLayer *vlayer, const QStringList &commitErrorsList )
{
  QStringList commitErrors = commitErrorsList;
  if ( vlayer && commitErrors.isEmpty() )
    commitErrors = vlayer->commitErrors();

  if ( vlayer && !vlayer->allowCommit() && commitErrors.empty() )
  {
    return;
  }

  const QString messageText = vlayer ? tr( "Could not commit changes to layer %1" ).arg( vlayer->name() )
                              : tr( "Could not commit changes" );

  QgsMessageViewer *mv = new QgsMessageViewer();
  mv->setWindowTitle( tr( "Commit Errors" ) );
  mv->setMessageAsPlainText( messageText
                             + "\n\n"
                             + tr( "Errors: %1\n" ).arg( commitErrors.join( QLatin1String( "\n  " ) ) )
                           );

  QToolButton *showMore = new QToolButton();
  QAction *act = new QAction( showMore );
  act->setText( tr( "Show more" ) );
  showMore->setStyleSheet( QStringLiteral( "background-color: rgba(255, 255, 255, 0); color: black; text-decoration: underline;" ) );
  showMore->setCursor( Qt::PointingHandCursor );
  showMore->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Preferred );
  showMore->addAction( act );
  showMore->setDefaultAction( act );
  connect( showMore, &QToolButton::triggered, mv, &QDialog::exec );
  connect( showMore, &QToolButton::triggered, showMore, &QObject::deleteLater );

  // no timeout set, since notice needs attention and is only shown first time layer is labeled
  QgsMessageBarItem *errorMsg = new QgsMessageBarItem(
    tr( "Commit errors" ),
    messageText,
    showMore,
    Qgis::MessageLevel::Warning,
    0,
    messageBar() );
  messageBar()->pushItem( errorMsg );
}

void QgisApp::labelingDialogFontNotFound( QAction *act )
{
  if ( !act )
  {
    return;
  }

  // get base pointer to layer
  QObject *obj = qvariant_cast<QObject *>( act->data() );

  // remove calling messagebar widget
  messageBar()->popWidget();

  if ( !obj )
  {
    return;
  }

  QgsMapLayer *layer = qobject_cast<QgsMapLayer *>( obj );
  if ( layer && setActiveLayer( layer ) )
  {
    labeling();
  }
}

void QgisApp::labeling()
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( activeLayer() );
  if ( !vlayer )
  {
    return;
  }

  mapStyleDock( true );
  mMapStyleWidget->setCurrentPage( QgsLayerStylingWidget::VectorLabeling );
}

void QgisApp::setMapStyleDockLayer( QgsMapLayer *layer )
{
  if ( !layer )
  {
    return;
  }

  mMapStyleWidget->setEnabled( true );
  // We don't set the layer if the dock isn't open mainly to save
  // the extra work if it's not needed
  if ( mMapStylingDock->isVisible() )
  {
    mMapStyleWidget->setLayer( layer );
  }
}

void QgisApp::mapStyleDock( bool enabled )
{
  mMapStylingDock->setUserVisible( enabled );
  setMapStyleDockLayer( activeLayer() );
}

void QgisApp::diagramProperties()
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( activeLayer() );
  if ( !vlayer )
  {
    visibleMessageBar()->pushMessage( tr( "Diagram Properties" ),
                                      tr( "Please select a vector layer first" ),
                                      Qgis::MessageLevel::Info
                                    );
    return;
  }

  QDialog dlg;
  dlg.setWindowTitle( tr( "Layer Diagram Properties" ) );
  QgsDiagramProperties *gui = new QgsDiagramProperties( vlayer, &dlg, mMapCanvas );
  gui->layout()->setContentsMargins( 0, 0, 0, 0 );
  QVBoxLayout *layout = new QVBoxLayout( &dlg );
  layout->addWidget( gui );

  QDialogButtonBox *buttonBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
    Qt::Horizontal, &dlg );
  layout->addWidget( buttonBox );

  dlg.setLayout( layout );

  connect( buttonBox->button( QDialogButtonBox::Ok ), &QAbstractButton::clicked,
           &dlg, &QDialog::accept );
  connect( buttonBox->button( QDialogButtonBox::Cancel ), &QAbstractButton::clicked,
           &dlg, &QDialog::reject );
  connect( buttonBox->button( QDialogButtonBox::Apply ), &QAbstractButton::clicked,
           gui, &QgsDiagramProperties::apply );

  if ( dlg.exec() )
    gui->apply();

  activateDeactivateLayerRelatedActions( vlayer );
}

void QgisApp::createAnnotationLayer()
{
  // pick a unique name for the layer
  QString name = tr( "Annotations" );
  int id = 1;
  while ( !QgsProject::instance()->mapLayersByName( name ).isEmpty() )
  {
    name = tr( "Annotations (%1)" ).arg( id );
    id++;
  }

  QgsAnnotationLayer::LayerOptions options( QgsProject::instance()->transformContext() );
  QgsAnnotationLayer *layer = new QgsAnnotationLayer( name, options );
  layer->setCrs( QgsProject::instance()->crs() );

  // layer should be created at top of layer tree
  QgsProject::instance()->addMapLayer( layer, false );
  QgsProject::instance()->layerTreeRoot()->insertLayer( 0, layer );
}

void QgisApp::setCadDockVisible( bool visible )
{
  mAdvancedDigitizingDockWidget->setVisible( visible );
}

void QgisApp::fieldCalculator()
{
  QgsVectorLayer *myLayer = qobject_cast<QgsVectorLayer *>( activeLayer() );
  if ( !myLayer )
  {
    return;
  }

  QgsFieldCalculator calc( myLayer, this );
  if ( calc.exec() )
  {
    myLayer->triggerRepaint();
  }
}

void QgisApp::attributeTable( QgsAttributeTableFilterModel::FilterMode filter )
{
  QgsVectorLayer *myLayer = qobject_cast<QgsVectorLayer *>( activeLayer() );
  if ( !myLayer || !myLayer->dataProvider() )
  {
    return;
  }

  QgsAttributeTableDialog *mDialog = new QgsAttributeTableDialog( myLayer, filter );
  mDialog->show();
  // the dialog will be deleted by itself on close
}

QString QgisApp::saveAsRasterFile( QgsRasterLayer *rasterLayer, const bool defaultAddToCanvas )
{
  if ( !rasterLayer )
    rasterLayer = qobject_cast<QgsRasterLayer *>( activeLayer() );

  if ( !rasterLayer )
  {
    return QString();
  }

  QgsRasterLayerSaveAsDialog d( rasterLayer, rasterLayer->dataProvider(),
                                mMapCanvas->extent(), rasterLayer->crs(),
                                mMapCanvas->mapSettings().destinationCrs(),
                                this );
  d.setAddToCanvas( defaultAddToCanvas );
  if ( d.exec() == QDialog::Rejected )
    return QString();

  QgsSettings settings;
  settings.setValue( QStringLiteral( "UI/lastRasterFileDir" ), QFileInfo( d.outputFileName() ).absolutePath() );

  QgsRasterFileWriter fileWriter( d.outputFileName() );
  if ( d.tileMode() )
  {
    fileWriter.setTiledMode( true );
    fileWriter.setMaxTileWidth( d.maximumTileSizeX() );
    fileWriter.setMaxTileHeight( d.maximumTileSizeY() );
  }
  else
  {
    fileWriter.setOutputFormat( d.outputFormat() );
  }

  // TODO: show error dialogs
  // TODO: this code should go somewhere else, but probably not into QgsRasterFileWriter
  // clone pipe/provider is not really necessary, ready for threads
  std::unique_ptr<QgsRasterPipe> pipe( nullptr );

  if ( d.mode() == QgsRasterLayerSaveAsDialog::RawDataMode )
  {
    QgsDebugMsgLevel( QStringLiteral( "Writing raw data" ), 2 );
    pipe.reset( new QgsRasterPipe() );
    if ( !pipe->set( rasterLayer->dataProvider()->clone() ) )
    {
      QgsDebugMsg( QStringLiteral( "Cannot set pipe provider" ) );
      return QString();
    }

    QgsRasterNuller *nuller = new QgsRasterNuller();
    for ( int band = 1; band <= rasterLayer->dataProvider()->bandCount(); band ++ )
    {
      nuller->setNoData( band, d.noData() );
    }
    if ( !pipe->insert( 1, nuller ) )
    {
      QgsDebugMsg( QStringLiteral( "Cannot set pipe nuller" ) );
      return QString();
    }

    // add projector if necessary
    if ( d.outputCrs() != rasterLayer->crs() )
    {
      QgsRasterProjector *projector = new QgsRasterProjector;
      projector->setCrs( rasterLayer->crs(), d.outputCrs(), QgsProject::instance()->transformContext() );
      if ( !pipe->insert( 2, projector ) )
      {
        QgsDebugMsg( QStringLiteral( "Cannot set pipe projector" ) );
        return QString();
      }
    }
  }
  else // RenderedImageMode
  {
    // clone the whole pipe
    QgsDebugMsgLevel( QStringLiteral( "Writing rendered image" ), 2 );
    pipe.reset( new QgsRasterPipe( *rasterLayer->pipe() ) );
    QgsRasterProjector *projector = pipe->projector();
    if ( !projector )
    {
      QgsDebugMsg( QStringLiteral( "Cannot get pipe projector" ) );
      return QString();
    }
    projector->setCrs( rasterLayer->crs(), d.outputCrs(), QgsProject::instance()->transformContext() );
  }

  if ( !pipe->last() )
  {
    return QString();
  }
  fileWriter.setCreateOptions( d.createOptions() );

  fileWriter.setBuildPyramidsFlag( d.buildPyramidsFlag() );
  fileWriter.setPyramidsList( d.pyramidsList() );
  fileWriter.setPyramidsResampling( d.pyramidsResamplingMethod() );
  fileWriter.setPyramidsFormat( d.pyramidsFormat() );
  fileWriter.setPyramidsConfigOptions( d.pyramidsConfigOptions() );

  bool tileMode = d.tileMode();
  bool addToCanvas = d.addToCanvas();
  QPointer< QgsRasterLayer > rlWeakPointer( rasterLayer );
  QString outputLayerName = d.outputLayerName();
  QString outputFormat = d.outputFormat();

  QgsRasterFileWriterTask *writerTask = new QgsRasterFileWriterTask( fileWriter, pipe.release(), d.nColumns(), d.nRows(),
      d.outputRectangle(), d.outputCrs(), QgsProject::instance()->transformContext() );

  // when writer is successful:

  connect( writerTask, &QgsRasterFileWriterTask::writeComplete, this,
           [this, tileMode, addToCanvas, rlWeakPointer, outputLayerName, outputFormat]( const QString & newFilename )
  {
    QString fileName = newFilename;
    if ( tileMode )
    {
      QFileInfo outputInfo( fileName );
      fileName = QStringLiteral( "%1/%2.vrt" ).arg( fileName, outputInfo.fileName() );
    }

    if ( addToCanvas )
    {
      if ( outputFormat == QLatin1String( "GPKG" ) && !outputLayerName.isEmpty() )
      {
        addRasterLayers( QStringList( QStringLiteral( "GPKG:%1:%2" ).arg( fileName, outputLayerName ) ) );
      }
      else
      {
        addRasterLayers( QStringList( fileName ) );
      }
    }
    if ( rlWeakPointer )
      emit layerSavedAs( rlWeakPointer, fileName );

    visibleMessageBar()->pushMessage( tr( "Layer Exported" ),
                                      tr( "Successfully saved raster layer to <a href=\"%1\">%2</a>" ).arg( QUrl::fromLocalFile( newFilename ).toString(), QDir::toNativeSeparators( newFilename ) ),
                                      Qgis::MessageLevel::Success, 0 );
  } );

  // when an error occurs:
  connect( writerTask, qOverload< int, const QString &>( &QgsRasterFileWriterTask::errorOccurred ), this, [ = ]( int error, const QString & errorMessage )
  {
    if ( error != QgsRasterFileWriter::WriteCanceled )
    {
      QString errorCodeStr;
      if ( error == QgsRasterFileWriter::SourceProviderError )
        errorCodeStr = tr( "source provider" );
      else if ( error == QgsRasterFileWriter::DestProviderError )
        errorCodeStr = tr( "destination provider" );
      else if ( error == QgsRasterFileWriter::CreateDatasourceError )
        errorCodeStr = tr( "data source creation" );
      else if ( error == QgsRasterFileWriter::WriteError )
        errorCodeStr = tr( "write error" );
      QString fullErrorMsg( tr( "Cannot write raster. Error code: %1" ).arg( errorCodeStr ) );
      if ( !errorMessage.isEmpty() )
        fullErrorMsg += "\n" + errorMessage;
      QMessageBox::warning( this, tr( "Save Raster" ),
                            fullErrorMsg,
                            QMessageBox::Ok );
    }
  } );

  QgsApplication::taskManager()->addTask( writerTask );
  return d.outputFileName();
}


QString QgisApp::saveAsFile( QgsMapLayer *layer, const bool onlySelected, const bool defaultToAddToMap )
{
  if ( !layer )
    layer = activeLayer();

  if ( !layer )
    return QString();

  QgsMapLayerType layerType = layer->type();
  switch ( layerType )
  {
    case QgsMapLayerType::RasterLayer:
      return saveAsRasterFile( qobject_cast<QgsRasterLayer *>( layer ), defaultToAddToMap );

    case QgsMapLayerType::VectorLayer:
      return saveAsVectorFileGeneral( qobject_cast<QgsVectorLayer *>( layer ), true, onlySelected, defaultToAddToMap );

    case QgsMapLayerType::MeshLayer:
    case QgsMapLayerType::VectorTileLayer:
    case QgsMapLayerType::PluginLayer:
    case QgsMapLayerType::AnnotationLayer:
    case QgsMapLayerType::PointCloudLayer:
    case QgsMapLayerType::GroupLayer:
      return QString();
  }
  return QString();
}

void QgisApp::makeMemoryLayerPermanent( QgsVectorLayer *layer )
{
  if ( !layer )
    return;

  const QString layerId = layer->id();

  auto onSuccess = [this, layerId]( const QString & newFilename,
                                    bool,
                                    const QString & newLayerName,
                                    const QString &,
                                    const QString & )
  {
    // we have to re-retrieve the layer, in case it's been removed during the lifetime of the writer task
    QgsVectorLayer *vl = qobject_cast< QgsVectorLayer * >( QgsProject::instance()->mapLayer( layerId ) );
    if ( vl )
    {
      QgsDataProvider::ProviderOptions options;
      QString source = newFilename;
      if ( ! newLayerName.isEmpty() )
        source += QStringLiteral( "|layername=%1" ).arg( newLayerName );
      vl->setDataSource( source, vl->name(), QStringLiteral( "ogr" ), options );
      vl->triggerRepaint();
      mLayerTreeView->refreshLayerSymbology( vl->id() );
      this->visibleMessageBar()->pushMessage( tr( "Layer Saved" ),
                                              tr( "Successfully saved scratch layer to <a href=\"%1\">%2</a>" ).arg( QUrl::fromLocalFile( newFilename ).toString(), QDir::toNativeSeparators( newFilename ) ),
                                              Qgis::MessageLevel::Success, 0 );
    }
  };

  auto onFailure = []( int error, const QString & errorMessage )
  {
    if ( error != QgsVectorFileWriter::Canceled )
    {
      QgsMessageViewer *m = new QgsMessageViewer( nullptr );
      m->setWindowTitle( tr( "Save Error" ) );
      m->setMessageAsPlainText( tr( "Could not make temporary scratch layer permanent.\nError: %1" ).arg( errorMessage ) );
      m->exec();
    }
  };

  saveAsVectorFileGeneral( layer, true, false, true, onSuccess, onFailure, QgsVectorLayerSaveAsDialog::Options(), tr( "Save Scratch Layer" ) );
}

void QgisApp::saveAsLayerDefinition()
{
  QgsSettings settings;
  QString lastUsedDir = settings.value( QStringLiteral( "UI/lastQLRDir" ), QDir::homePath() ).toString();

  QString path = QFileDialog::getSaveFileName( this, QStringLiteral( "Save as Layer Definition File" ), lastUsedDir, QStringLiteral( "*.qlr" ) );
  QgsDebugMsgLevel( path, 2 );
  if ( path.isEmpty() )
    return;

  QString errorMessage;
  bool saved = QgsLayerDefinition::exportLayerDefinition( path, mLayerTreeView->selectedNodes(), errorMessage );
  if ( !saved )
  {
    visibleMessageBar()->pushMessage( tr( "Error saving layer definition file" ), errorMessage, Qgis::MessageLevel::Warning );
  }

  QFileInfo fi( path );
  settings.setValue( QStringLiteral( "UI/lastQLRDir" ), fi.path() );
}

void QgisApp::saveStyleFile( QgsMapLayer *layer )
{
  if ( !layer )
  {
    layer = activeLayer();
  }

  if ( !layer || !layer->dataProvider() )
    return;

  switch ( layer->type() )
  {

    case QgsMapLayerType::VectorLayer:
    {
      QgsVectorLayer *vlayer = qobject_cast< QgsVectorLayer * >( layer );
      QgsVectorLayerSaveStyleDialog dlg( vlayer, this );

      if ( dlg.exec() )
      {
        bool resultFlag = false;

        QgsVectorLayerProperties::StyleType type = dlg.currentStyleType();
        switch ( type )
        {
          case QgsVectorLayerProperties::QML:
          case QgsVectorLayerProperties::SLD:
          {
            QString message;
            QString filePath = dlg.outputFilePath();
            if ( type == QgsVectorLayerProperties::QML )
              message = vlayer->saveNamedStyle( filePath, resultFlag, dlg.styleCategories() );
            else
              message = vlayer->saveSldStyle( filePath, resultFlag );

            if ( resultFlag )
            {
              mInfoBar->pushMessage( tr( "Style saved" ), tr( "Successfully exported style to <a href=\"%1\">%2</a>" ).arg( QUrl::fromLocalFile( filePath ).toString(), QDir::toNativeSeparators( filePath ) ), Qgis::MessageLevel::Success, 0 );
            }
            else
            {
              mInfoBar->pushMessage( tr( "Save Style" ), message, Qgis::MessageLevel::Warning );
            }

            break;
          }
          case QgsVectorLayerProperties::DB:
          {
            QString infoWindowTitle = tr( "Save style to DB (%1)" ).arg( vlayer->providerType() );
            QString msgError;

            QgsVectorLayerSaveStyleDialog::SaveToDbSettings dbSettings = dlg.saveToDbSettings();

            QString errorMessage;
            if ( QgsProviderRegistry::instance()->styleExists( vlayer->providerType(), vlayer->source(), dbSettings.name, errorMessage ) )
            {
              if ( QMessageBox::question( nullptr, tr( "Save style in database" ),
                                          tr( "A matching style already exists in the database for this layer. Do you want to overwrite it?" ),
                                          QMessageBox::Yes | QMessageBox::No ) == QMessageBox::No )
              {
                return;
              }
            }
            else if ( !errorMessage.isEmpty() )
            {
              mInfoBar->pushMessage( infoWindowTitle, errorMessage, Qgis::MessageLevel::Warning );
              return;
            }

            vlayer->saveStyleToDatabase( dbSettings.name, dbSettings.description, dbSettings.isDefault, dbSettings.uiFileContent, msgError );

            if ( !msgError.isNull() )
            {
              mInfoBar->pushMessage( infoWindowTitle, msgError, Qgis::MessageLevel::Warning );
            }
            else
            {
              mInfoBar->pushMessage( infoWindowTitle, tr( "Style saved" ), Qgis::MessageLevel::Success );
            }
            break;
          }
        }
      }
      break;
    }

    case QgsMapLayerType::RasterLayer:
    case QgsMapLayerType::MeshLayer:
    case QgsMapLayerType::PointCloudLayer:
    case QgsMapLayerType::VectorTileLayer:
    {
      QgsSettings settings;
      QString lastUsedDir = settings.value( QStringLiteral( "style/lastStyleDir" ), QDir::homePath() ).toString();
      QString filename = QFileDialog::getSaveFileName( this,
                         tr( "Save as QGIS Layer Style File" ),
                         lastUsedDir,
                         tr( "QGIS Layer Style File" ) + " (*.qml)" );
      if ( filename.isEmpty() )
        return;

      if ( ! filename.endsWith( QLatin1String( ".qml" ) ) )
      {
        filename += QLatin1String( ".qml" );
      }

      bool defaultLoadedFlag;
      layer->saveNamedStyle( filename, defaultLoadedFlag );

      settings.setValue( QStringLiteral( "style/lastStyleDir" ), filename );
      break;
    }

    case QgsMapLayerType::AnnotationLayer:
    case QgsMapLayerType::PluginLayer:
    case QgsMapLayerType::GroupLayer:
      break;

  }
}

///@cond PRIVATE

/**
 * Field value converter for export as vector layer
 * \note Not available in Python bindings
 */
class QgisAppFieldValueConverter : public QgsVectorFileWriter::FieldValueConverter
{
  public:
    QgisAppFieldValueConverter( QgsVectorLayer *vl, const QgsAttributeList &attributesAsDisplayedValues );

    QgsField fieldDefinition( const QgsField &field ) override;

    QVariant convert( int idx, const QVariant &value ) override;

    QgisAppFieldValueConverter *clone() const override;

  private:
    QPointer< QgsVectorLayer > mLayer;
    QgsAttributeList mAttributesAsDisplayedValues;
};

QgisAppFieldValueConverter::QgisAppFieldValueConverter( QgsVectorLayer *vl, const QgsAttributeList &attributesAsDisplayedValues )
  : mLayer( vl )
  , mAttributesAsDisplayedValues( attributesAsDisplayedValues )
{
}

QgsField QgisAppFieldValueConverter::fieldDefinition( const QgsField &field )
{
  if ( !mLayer )
    return field;

  int idx = mLayer->fields().indexFromName( field.name() );
  if ( mAttributesAsDisplayedValues.contains( idx ) )
  {
    return QgsField( field.name(), QVariant::String );
  }
  return field;
}

QVariant QgisAppFieldValueConverter::convert( int idx, const QVariant &value )
{
  if ( !mLayer || !mAttributesAsDisplayedValues.contains( idx ) )
  {
    return value;
  }
  const QgsEditorWidgetSetup setup = QgsGui::editorWidgetRegistry()->findBest( mLayer, mLayer->fields().field( idx ).name() );
  QgsFieldFormatter *fieldFormatter = QgsApplication::fieldFormatterRegistry()->fieldFormatter( setup.type() );
  return fieldFormatter->representValue( mLayer, idx, setup.config(), QVariant(), value );
}

QgisAppFieldValueConverter *QgisAppFieldValueConverter::clone() const
{
  return new QgisAppFieldValueConverter( *this );
}

///@endcond

QString QgisApp::saveAsVectorFileGeneral( QgsVectorLayer *vlayer, bool symbologyOption, bool onlySelected, bool defaultToAddToMap )
{
  if ( !vlayer )
  {
    vlayer = qobject_cast<QgsVectorLayer *>( activeLayer() ); // FIXME: output of multiple layers at once?
  }

  if ( !vlayer )
    return QString();

  const QString layerId = vlayer->id();

  auto onSuccess = [this, layerId]( const QString & newFilename,
                                    bool addToCanvas,
                                    const QString & layerName,
                                    const QString & encoding,
                                    const QString & vectorFileName )
  {
    if ( addToCanvas )
    {
      QString uri( newFilename );
      if ( !layerName.isEmpty() )
        uri += "|layername=" + layerName;
      this->addVectorLayers( QStringList( uri ), encoding, QStringLiteral( "file" ) );
    }

    // We need to re-retrieve the map layer here, in case it's been deleted during the lifetime of the task
    if ( QgsVectorLayer *vlayer = qobject_cast< QgsVectorLayer * >( QgsProject::instance()->mapLayer( layerId ) ) )
      this->emit layerSavedAs( vlayer, vectorFileName );

    this->visibleMessageBar()->pushMessage( tr( "Layer Exported" ),
                                            tr( "Successfully saved vector layer to <a href=\"%1\">%2</a>" ).arg( QUrl::fromLocalFile( newFilename ).toString(), QDir::toNativeSeparators( newFilename ) ),
                                            Qgis::MessageLevel::Success, 0 );
  };

  auto onFailure = []( int error, const QString & errorMessage )
  {
    if ( error != QgsVectorFileWriter::Canceled )
    {
      QgsMessageViewer *m = new QgsMessageViewer( nullptr );
      m->setWindowTitle( tr( "Save Error" ) );
      m->setMessageAsPlainText( tr( "Export to vector file failed.\nError: %1" ).arg( errorMessage ) );
      m->exec();
    }
  };

  return saveAsVectorFileGeneral( vlayer, symbologyOption, onlySelected, defaultToAddToMap, onSuccess, onFailure );
}

QString QgisApp::saveAsVectorFileGeneral( QgsVectorLayer *vlayer, bool symbologyOption, bool onlySelected, bool defaultToAddToMap, const std::function<void( const QString &, bool, const QString &, const QString &, const QString & )> &onSuccess, const std::function<void ( int, const QString & )> &onFailure, QgsVectorLayerSaveAsDialog::Options options, const QString &dialogTitle )
{
  QgsCoordinateReferenceSystem destCRS;

  if ( !symbologyOption )
  {
    options &= ~QgsVectorLayerSaveAsDialog::Symbology;
  }

  QgsVectorLayerSaveAsDialog *dialog = new QgsVectorLayerSaveAsDialog( vlayer, options, this );
  if ( !dialogTitle.isEmpty() )
    dialog->setWindowTitle( dialogTitle );

  dialog->setMapCanvas( mMapCanvas );
  dialog->setIncludeZ( QgsWkbTypes::hasZ( vlayer->wkbType() ) );
  dialog->setOnlySelected( onlySelected );
  dialog->setAddToCanvas( defaultToAddToMap );

  QString vectorFilename;
  if ( dialog->exec() == QDialog::Accepted )
  {
    QString encoding = dialog->encoding();
    vectorFilename = dialog->filename();
    QString format = dialog->format();
    QStringList datasourceOptions = dialog->datasourceOptions();
    bool autoGeometryType = dialog->automaticGeometryType();
    QgsWkbTypes::Type forcedGeometryType = dialog->geometryType();

    QgsCoordinateTransform ct;
    destCRS = dialog->crsObject();

    if ( destCRS.isValid() )
    {
      QgsDatumTransformDialog::run( vlayer->crs(), destCRS, this, mMapCanvas );
      ct = QgsCoordinateTransform( vlayer->crs(), destCRS, QgsProject::instance() );
    }

    QgsRectangle filterExtent = dialog->filterExtent();
    QgisAppFieldValueConverter converter( vlayer, dialog->attributesAsDisplayedValues() );
    QgisAppFieldValueConverter *converterPtr = nullptr;
    // No need to use the converter if there is nothing to convert
    if ( !dialog->attributesAsDisplayedValues().isEmpty() )
      converterPtr = &converter;

    QgsVectorFileWriter::SaveVectorOptions options;
    options.driverName = format;
    options.layerName = dialog->layername();
    options.actionOnExistingFile = dialog->creationActionOnExistingFile();
    options.fileEncoding = encoding;
    options.ct = ct;
    options.onlySelectedFeatures = dialog->onlySelected();
    options.datasourceOptions = datasourceOptions;
    options.layerOptions = dialog->layerOptions();
    options.skipAttributeCreation = dialog->selectedAttributes().isEmpty();
    options.symbologyExport = static_cast< QgsVectorFileWriter::SymbologyExport >( dialog->symbologyExport() );
    options.symbologyScale = dialog->scale();
    if ( dialog->hasFilterExtent() )
      options.filterExtent = filterExtent;
    options.overrideGeometryType = autoGeometryType ? QgsWkbTypes::Unknown : forcedGeometryType;
    options.forceMulti = dialog->forceMulti();
    options.includeZ = dialog->includeZ();
    options.attributes = dialog->selectedAttributes();
    options.attributesExportNames = dialog->attributesExportNames();
    options.fieldValueConverter = converterPtr;
    options.saveMetadata = dialog->persistMetadata();
    options.layerMetadata = vlayer->metadata();

    bool addToCanvas = dialog->addToCanvas();
    QgsVectorFileWriterTask *writerTask = new QgsVectorFileWriterTask( vlayer, vectorFilename, options );

    // when writer is successful:
    connect( writerTask, &QgsVectorFileWriterTask::completed, this, [onSuccess, addToCanvas, encoding, vectorFilename]( const QString & newFilename, const QString & newLayer )
    {
      onSuccess( newFilename, addToCanvas, newLayer, encoding, vectorFilename );
    } );

    // when an error occurs:
    connect( writerTask, &QgsVectorFileWriterTask::errorOccurred, this, [onFailure]( int error, const QString & errorMessage )
    {
      onFailure( error, errorMessage );
    } );

    QgsApplication::taskManager()->addTask( writerTask );
  }

  delete dialog;
  return vectorFilename;
}

void QgisApp::layerProperties()
{
  showLayerProperties( activeLayer() );
}

void QgisApp::deleteSelected( QgsMapLayer *layer, QWidget *parent, bool checkFeaturesVisible )
{
  if ( !layer )
  {
    layer = mLayerTreeView->currentLayer();
  }

  if ( !parent )
  {
    parent = this;
  }

  if ( !layer )
  {
    visibleMessageBar()->pushMessage( tr( "No Layer Selected" ),
                                      tr( "To delete features, you must select a vector layer in the legend" ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
  if ( !vlayer )
  {
    visibleMessageBar()->pushMessage( tr( "No Vector Layer Selected" ),
                                      tr( "Deleting features only works on vector layers" ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  if ( !( vlayer->dataProvider()->capabilities() & QgsVectorDataProvider::DeleteFeatures ) )
  {
    visibleMessageBar()->pushMessage( tr( "Provider does not support deletion" ),
                                      tr( "Data provider does not support deleting features" ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  if ( !vlayer->isEditable() )
  {
    visibleMessageBar()->pushMessage( tr( "Layer not editable" ),
                                      tr( "The current layer is not editable. Choose 'Start editing' in the digitizing toolbar." ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  //validate selection
  const int numberOfSelectedFeatures = vlayer->selectedFeatureCount();
  if ( numberOfSelectedFeatures == 0 )
  {
    visibleMessageBar()->pushMessage( tr( "No Features Selected" ),
                                      tr( "The current layer has no selected features" ),
                                      Qgis::MessageLevel::Info );
    return;
  }
  //display a warning
  if ( checkFeaturesVisible )
  {
    QgsFeature feat;
    QgsFeatureIterator it = vlayer->getSelectedFeatures( QgsFeatureRequest().setNoAttributes() );
    bool allFeaturesInView = true;
    QgsRectangle viewRect = mMapCanvas->mapSettings().mapToLayerCoordinates( vlayer, mMapCanvas->extent() );

    while ( it.nextFeature( feat ) )
    {
      if ( allFeaturesInView && !viewRect.intersects( feat.geometry().boundingBox() ) )
      {
        allFeaturesInView = false;
        break;
      }
    }

    if ( !allFeaturesInView )
    {
      // for extra safety to make sure we are not removing geometries by accident
      int res = QMessageBox::warning( mMapCanvas, tr( "Delete %n feature(s) from layer \"%1\"", nullptr, numberOfSelectedFeatures ).arg( vlayer->name() ),
                                      tr( "Some of the selected features are outside of the current map view. Would you still like to continue?" ),
                                      QMessageBox::Yes | QMessageBox::No );
      if ( res != QMessageBox::Yes )
        return;
    }
  }

  QgsVectorLayerUtils::QgsDuplicateFeatureContext infoContext;
  if ( QgsVectorLayerUtils::impactsCascadeFeatures( vlayer, vlayer->selectedFeatureIds(), QgsProject::instance(), infoContext, QgsVectorLayerUtils::IgnoreAuxiliaryLayers ) )
  {
    QString childrenInfo;
    int childrenCount = 0;
    const auto infoContextLayers = infoContext.layers();
    for ( QgsVectorLayer *chl : infoContextLayers )
    {
      childrenCount += infoContext.duplicatedFeatures( chl ).size();
      childrenInfo += ( tr( "%n feature(s) on layer \"%1\", ", nullptr, infoContext.duplicatedFeatures( chl ).size() ).arg( chl->name() ) );
    }

    // for extra safety to make sure we know that the delete can have impact on children and joins
    int res = QMessageBox::question( mMapCanvas, tr( "Delete at least %1 feature(s) on other layer(s)" ).arg( childrenCount ),
                                     tr( "Delete %1 feature(s) on layer \"%2\", %3 as well\nand all of its other descendants.\nDelete these features?" ).arg( numberOfSelectedFeatures ).arg( vlayer->name() ).arg( childrenInfo ),
                                     QMessageBox::Yes | QMessageBox::No );
    if ( res != QMessageBox::Yes )
      return;
  }

  vlayer->beginEditCommand( tr( "Features deleted" ) );
  int deletedCount = 0;
  QgsVectorLayer::DeleteContext context( true, QgsProject::instance() );
  if ( !vlayer->deleteSelectedFeatures( &deletedCount, &context ) )
  {
    visibleMessageBar()->pushMessage( tr( "Problem deleting features" ),
                                      tr( "A problem occurred during deletion from layer \"%1\". %n feature(s) not deleted.", nullptr, numberOfSelectedFeatures - deletedCount ).arg( vlayer->name() ),
                                      Qgis::MessageLevel::Warning );
  }
  else
  {
    const QList<QgsVectorLayer *> contextLayers = context.handledLayers( false );
    // if it affects more than one non-auxiliary layer, print feedback for all descendants
    if ( contextLayers.size() > 1 )
    {
      deletedCount = 0;
      QString feedbackMessage;
      for ( QgsVectorLayer *contextLayer : contextLayers )
      {
        feedbackMessage += tr( "%1 on layer %2. " ).arg( context.handledFeatures( contextLayer ).size() ).arg( contextLayer->name() );
        deletedCount += context.handledFeatures( contextLayer ).size();
      }
      visibleMessageBar()->pushMessage( tr( "%n feature(s) deleted: %1", nullptr, deletedCount ).arg( feedbackMessage ), Qgis::MessageLevel::Success );
    }

    showStatusMessage( tr( "%n feature(s) deleted.", "number of features deleted", deletedCount ) );
  }

  vlayer->endEditCommand();
}

void QgisApp::moveFeature()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::MoveFeature ) );
}

void QgisApp::moveFeatureCopy()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::MoveFeatureCopy ) );
}

void QgisApp::offsetCurve()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::OffsetCurve ) );
}

void QgisApp::simplifyFeature()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::SimplifyFeature ) );
}

void QgisApp::deleteRing()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::DeleteRing ) );
}

void QgisApp::deletePart()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::DeletePart ) );
}

void QgisApp::reverseLine()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::ReverseLine ) );
}

QgsGeometry QgisApp::unionGeometries( const QgsVectorLayer *vl, QgsFeatureList &featureList, bool &canceled )
{
  canceled = false;
  if ( !vl || featureList.size() < 2 )
  {
    return QgsGeometry();
  }

  if ( !featureList.at( 0 ).hasGeometry() )
    return QgsGeometry();

  QgsGeometry unionGeom = featureList.at( 0 ).geometry();

  QProgressDialog progress( tr( "Merging features…" ), tr( "Abort" ), 0, featureList.size(), this );
  progress.setWindowModality( Qt::WindowModal );

  QApplication::setOverrideCursor( Qt::WaitCursor );

  for ( int i = 1; i < featureList.size(); ++i )
  {
    if ( progress.wasCanceled() )
    {
      QApplication::restoreOverrideCursor();
      canceled = true;
      return QgsGeometry();
    }
    progress.setValue( i );
    QgsGeometry currentGeom = featureList.at( i ).geometry();
    if ( !currentGeom.isNull() )
    {
      unionGeom = unionGeom.combine( currentGeom );
      if ( unionGeom.isNull() )
      {
        QApplication::restoreOverrideCursor();
        return QgsGeometry();
      }
    }
  }

  //convert unionGeom to a multipart geometry in case it is necessary to match the layer type
  if ( QgsWkbTypes::isMultiType( vl->wkbType() ) && !unionGeom.isMultipart() )
  {
    unionGeom.convertToMultiType();
  }

  QApplication::restoreOverrideCursor();
  progress.setValue( featureList.size() );
  return unionGeom;
}

bool QgisApp::uniqueLayoutTitle( QWidget *parent, QString &title, bool acceptEmpty, QgsMasterLayoutInterface::Type type, const QString &currentTitle )
{
  if ( !parent )
  {
    parent = this;
  }
  bool titleValid = false;
  QString newTitle = QString( currentTitle );

  QString typeString;
  QString helpPage;
  switch ( type )
  {
    case QgsMasterLayoutInterface::PrintLayout:
      typeString = tr( "print layout" );
      helpPage = QStringLiteral( "print_composer/index.html" );
      break;
    case QgsMasterLayoutInterface::Report:
      typeString = tr( "report" );
      helpPage = QStringLiteral( "print_composer/create_reports.html" );
      break;
  }

  QString chooseMsg = tr( "Enter a unique %1 title" ).arg( typeString );
  if ( acceptEmpty )
  {
    chooseMsg += '\n' + tr( "(a title will be automatically generated if left empty)" );
  }
  QString titleMsg = chooseMsg;

  QStringList layoutNames;
  const QList< QgsMasterLayoutInterface * > layouts = QgsProject::instance()->layoutManager()->layouts();
  layoutNames.reserve( layouts.size() + 1 );
  for ( QgsMasterLayoutInterface *l : layouts )
  {
    layoutNames << l->name();
  }

  const QString windowTitle = tr( "Create %1" ).arg( QgsGui::higFlags() & QgsGui::HigDialogTitleIsTitleCase ? QgsStringUtils::capitalize( typeString, Qgis::Capitalization::TitleCase )
                              : typeString );

  while ( !titleValid )
  {

    QgsNewNameDialog dlg( typeString, newTitle, QStringList(), layoutNames, Qt::CaseSensitive, parent );
    dlg.setWindowTitle( windowTitle );
    dlg.setHintString( titleMsg );
    dlg.setOverwriteEnabled( false );
    dlg.setAllowEmptyName( true );
    dlg.setConflictingNameWarning( tr( "Title already exists!" ) );

    dlg.buttonBox()->addButton( QDialogButtonBox::Help );
    connect( dlg.buttonBox(), &QDialogButtonBox::helpRequested, this, [ = ]
    {
      QgsHelp::openHelp( helpPage );
    } );

    if ( dlg.exec() != QDialog::Accepted )
    {
      return false;
    }

    newTitle = dlg.name();
    if ( newTitle.isEmpty() )
    {
      if ( !acceptEmpty )
      {
        titleMsg = chooseMsg + "\n\n" + tr( "Title can not be empty!" );
      }
      else
      {
        titleValid = true;
        newTitle = QgsProject::instance()->layoutManager()->generateUniqueTitle( type );
      }
    }
    else if ( layoutNames.indexOf( newTitle, 1 ) >= 0 )
    {
      layoutNames[0] = QString(); // clear non-unique name
      titleMsg = chooseMsg + "\n\n" + tr( "Title already exists!" );
    }
    else
    {
      titleValid = true;
    }
  }

  title = newTitle;

  return true;
}

QgsLayoutDesignerDialog *QgisApp::createNewPrintLayout( const QString &t )
{
  QString title = t;
  if ( title.isEmpty() )
  {
    title = QgsProject::instance()->layoutManager()->generateUniqueTitle( QgsMasterLayoutInterface::PrintLayout );
  }
  //create new layout object
  QgsPrintLayout *layout = new QgsPrintLayout( QgsProject::instance() );
  layout->setName( title );
  layout->initializeDefaults();
  if ( QgsProject::instance()->layoutManager()->addLayout( layout ) )
    return openLayoutDesignerDialog( layout );
  else
    return nullptr;
}

QgsLayoutDesignerDialog *QgisApp::createNewReport( QString title )
{
  if ( title.isEmpty() )
  {
    title = QgsProject::instance()->layoutManager()->generateUniqueTitle( QgsMasterLayoutInterface::Report );
  }
  //create new report
  std::unique_ptr< QgsReport > report = std::make_unique< QgsReport >( QgsProject::instance() );
  report->setName( title );
  QgsMasterLayoutInterface *layout = report.get();
  QgsProject::instance()->layoutManager()->addLayout( report.release() );
  return openLayoutDesignerDialog( layout );
}

QgsLayoutDesignerDialog *QgisApp::openLayoutDesignerDialog( QgsMasterLayoutInterface *layout )
{
  // maybe a designer already open for this layout
  const auto constMLayoutDesignerDialogs = mLayoutDesignerDialogs;
  for ( QgsLayoutDesignerDialog *designer : constMLayoutDesignerDialogs )
  {
    if ( designer->masterLayout() == layout )
    {
      designer->show();
      designer->activate();
      designer->raise();
      return designer;
    }
  }

  //nope, so make a new one
  //important - no parent set, otherwise Windows 10 sets the dialog as always on top of the QGIS window!!
  QgsLayoutDesignerDialog *newDesigner = new QgsLayoutDesignerDialog( nullptr );
  newDesigner->setMasterLayout( layout );
  connect( newDesigner, &QgsLayoutDesignerDialog::aboutToClose, this, [this, newDesigner]
  {
    emit layoutDesignerWillBeClosed( newDesigner->iface() );
    mLayoutDesignerDialogs.remove( newDesigner );
    emit layoutDesignerClosed();
  } );

  //add it to the map of existing print designers
  mLayoutDesignerDialogs.insert( newDesigner );

  newDesigner->open();
  emit layoutDesignerOpened( newDesigner->iface() );

  return newDesigner;
}

QgsLayoutDesignerDialog *QgisApp::duplicateLayout( QgsMasterLayoutInterface *layout, const QString &t )
{
  QString title = t;
  if ( title.isEmpty() )
  {
    // TODO: inject a bit of randomness in auto-titles?
    title = tr( "%1 copy" ).arg( layout->name() );
  }

  QgsMasterLayoutInterface *newLayout = QgsProject::instance()->layoutManager()->duplicateLayout( layout, title );
  QgsLayoutDesignerDialog *dlg = openLayoutDesignerDialog( newLayout );
  dlg->activate();
  return dlg;
}

void QgisApp::deleteLayoutDesigners()
{
  // need a copy, since mLayoutDesignerDialogs will be modified as we iterate
  const QSet<QgsLayoutDesignerDialog *> dialogs = mLayoutDesignerDialogs;
  for ( QgsLayoutDesignerDialog *dlg : dialogs )
  {
    dlg->close(); // will trigger delete
  }
}

void QgisApp::setupLayoutManagerConnections()
{
  QgsLayoutManager *manager = QgsProject::instance()->layoutManager();
  connect( manager, &QgsLayoutManager::layoutAdded, this, [ = ]( const QString & name )
  {
    QgsMasterLayoutInterface *l = QgsProject::instance()->layoutManager()->layoutByName( name );
    if ( !l )
      return;
    QgsPrintLayout *pl = dynamic_cast< QgsPrintLayout *>( l );
    if ( !pl )
      return;

    mAtlasFeatureActions.insert( pl, nullptr );
    connect( pl, &QgsPrintLayout::nameChanged, this, [this, pl]( const QString & name )
    {
      QgsMapLayerAction *action = mAtlasFeatureActions.value( pl );
      if ( action )
      {
        action->setText( tr( "Set as atlas feature for %1" ).arg( name ) );
      }
    } );

    connect( pl->atlas(), &QgsLayoutAtlas::coverageLayerChanged, this, [this, pl]( QgsVectorLayer * coverageLayer )
    {
      setupAtlasMapLayerAction( pl, static_cast< bool >( coverageLayer ) );
    } );

    connect( pl->atlas(), &QgsLayoutAtlas::toggled, this, [this, pl]( bool enabled )
    {
      setupAtlasMapLayerAction( pl, enabled );
    } );

    setupAtlasMapLayerAction( pl, pl->atlas()->enabled() && pl->atlas()->coverageLayer() );
  } );

  connect( manager, &QgsLayoutManager::layoutAboutToBeRemoved, this, [ = ]( const QString & name )
  {
    QgsMasterLayoutInterface *l = QgsProject::instance()->layoutManager()->layoutByName( name );
    if ( l )
    {
      QgsPrintLayout *pl = dynamic_cast< QgsPrintLayout * >( l );
      if ( pl )
      {
        QgsMapLayerAction *action = mAtlasFeatureActions.value( pl );
        if ( action )
        {
          QgsGui::mapLayerActionRegistry()->removeMapLayerAction( action );
          delete action;
          mAtlasFeatureActions.remove( pl );
        }
      }
    }
  } );
}

Qgs3DMapCanvasWidget *QgisApp::open3DMapView( const QString &viewName )
{
#ifdef HAVE_3D
  QgsReadWriteContext readWriteContext;
  readWriteContext.setPathResolver( QgsProject::instance()->pathResolver() );

  QDomElement elem3DMap = QgsProject::instance()->viewsManager()->get3DViewSettings( viewName );

  if ( elem3DMap.isNull() )
    return nullptr;

  bool isDocked = elem3DMap.attribute( QStringLiteral( "isDocked" ), "1" ).toInt() == 1;
  Qgs3DMapCanvasWidget *mapCanvas3D = createNew3DMapCanvasDock( viewName, isDocked );
  if ( !mapCanvas3D )
    return nullptr;

  read3DMapViewSettings( mapCanvas3D, elem3DMap );

  QgsProject::instance()->viewsManager()->set3DViewInitiallyVisible( viewName, true );

  return mapCanvas3D;
#else
  Q_UNUSED( viewName );
  return nullptr;
#endif
}

void QgisApp::close3DMapView( const QString &viewName )
{
#ifdef HAVE_3D
  Qgs3DMapCanvasWidget *widget = get3DMapView( viewName );
  if ( !widget )
    return;
  mOpen3DMapViews.remove( widget );

  QDomImplementation DomImplementation;
  QDomDocumentType documentType =
    DomImplementation.createDocumentType(
      QStringLiteral( "qgis" ), QStringLiteral( "http://mrcc.com/qgis.dtd" ), QStringLiteral( "SYSTEM" ) );
  QDomDocument doc( documentType );

  if ( !QgsProject::instance()->viewsManager()->get3DViewSettings( viewName ).isNull() )
  {
    QDomElement elem3DMap;
    elem3DMap = doc.createElement( QStringLiteral( "view" ) );
    write3DMapViewSettings( widget, doc, elem3DMap );

    QgsProject::instance()->viewsManager()->register3DViewSettings( viewName, elem3DMap );
    QgsProject::instance()->viewsManager()->set3DViewInitiallyVisible( viewName, false );
  }
  widget->deleteLater();
#else
  Q_UNUSED( viewName );
#endif
}

Qgs3DMapCanvasWidget *QgisApp::duplicate3DMapView( const QString &existingViewName, const QString &newViewName )
{
#ifdef HAVE_3D
  QgsReadWriteContext readWriteContext;
  readWriteContext.setPathResolver( QgsProject::instance()->pathResolver() );

  QDomElement existingViewDom = QgsProject::instance()->viewsManager()->get3DViewSettings( existingViewName );
  bool isDocked = existingViewDom.attribute( QStringLiteral( "isDocked" ), "1" ).toInt() == 1;
  Qgs3DMapCanvasWidget *newCanvasWidget = createNew3DMapCanvasDock( newViewName, isDocked );
  if ( !newCanvasWidget )
    return nullptr;

  QDomImplementation DomImplementation;
  QDomDocumentType documentType =
    DomImplementation.createDocumentType(
      QStringLiteral( "qgis" ), QStringLiteral( "http://mrcc.com/qgis.dtd" ), QStringLiteral( "SYSTEM" ) );
  QDomDocument doc( documentType );

  // If the 3D view is open, copy its configuration to the duplicate widget, otherwise just use the recorded
  // settings from m3DMapViewsWidgets
  if ( Qgs3DMapCanvasWidget *widget = get3DMapView( existingViewName ) )
  {
    Qgs3DMapSettings *map = new Qgs3DMapSettings( *widget->mapCanvas3D()->map() );
    newCanvasWidget->setMapSettings( map );

    newCanvasWidget->mapCanvas3D()->cameraController()->readXml( widget->mapCanvas3D()->cameraController()->writeXml( doc ) );
    newCanvasWidget->animationWidget()->setAnimation( widget->animationWidget()->animation() );

    connect( QgsProject::instance(), &QgsProject::transformContextChanged, map, [map]
    {
      map->setTransformContext( QgsProject::instance()->transformContext() );
    } );
  }
  else
  {
    existingViewDom.setAttribute( QStringLiteral( "name" ), newViewName );
    read3DMapViewSettings( newCanvasWidget, existingViewDom );
  }

  QDomElement elem3DMap;
  elem3DMap = doc.createElement( QStringLiteral( "view" ) );
  write3DMapViewSettings( newCanvasWidget, doc, elem3DMap );

  QgsProject::instance()->viewsManager()->register3DViewSettings( newViewName, elem3DMap );
  QgsProject::instance()->viewsManager()->set3DViewInitiallyVisible( newViewName, true );

  return newCanvasWidget;
#else
  Q_UNUSED( existingViewName )
  Q_UNUSED( newViewName )
  return nullptr;
#endif
}

Qgs3DMapCanvasWidget *QgisApp::get3DMapView( const QString &viewName )
{
#ifdef HAVE_3D
  for ( Qgs3DMapCanvasWidget *w : mOpen3DMapViews )
  {
    if ( w->canvasName() == viewName )
      return w;
  }
#else
  Q_UNUSED( viewName )
#endif
  return nullptr;
}

QList<Qgs3DMapCanvasWidget *> QgisApp::get3DMapViews()
{
  QList<Qgs3DMapCanvasWidget *> views;
#ifdef HAVE_3D
  for ( Qgs3DMapCanvasWidget *w : mOpen3DMapViews )
    views.append( w );
#endif
  return views;
}

void QgisApp::setupDuplicateFeaturesAction()
{
  mDuplicateFeatureAction.reset( new QgsMapLayerAction( tr( "Duplicate Feature" ),
                                 nullptr, QgsMapLayerAction::SingleFeature,
                                 QgsApplication::getThemeIcon( QStringLiteral( "/mActionDuplicateFeature.svg" ) ), QgsMapLayerAction::EnabledOnlyWhenEditable ) );

  QgsGui::mapLayerActionRegistry()->addMapLayerAction( mDuplicateFeatureAction.get() );
  connect( mDuplicateFeatureAction.get(), &QgsMapLayerAction::triggeredForFeature, this, [this]( QgsMapLayer * layer, const QgsFeature & feat )
  {
    duplicateFeatures( layer, feat );
  }
         );

  mDuplicateFeatureDigitizeAction.reset( new QgsMapLayerAction( tr( "Duplicate Feature and Digitize" ),
                                         nullptr, QgsMapLayerAction::SingleFeature,
                                         QgsApplication::getThemeIcon( QStringLiteral( "/mActionDuplicateFeatureDigitized.svg" ) ), QgsMapLayerAction::EnabledOnlyWhenEditable ) );

  QgsGui::mapLayerActionRegistry()->addMapLayerAction( mDuplicateFeatureDigitizeAction.get() );
  connect( mDuplicateFeatureDigitizeAction.get(), &QgsMapLayerAction::triggeredForFeature, this, [this]( QgsMapLayer * layer, const QgsFeature & feat )
  {
    duplicateFeatureDigitized( layer, feat );
  }
         );
}

void QgisApp::setupAtlasMapLayerAction( QgsPrintLayout *layout, bool enableAction )
{
  QgsMapLayerAction *action = mAtlasFeatureActions.value( layout );
  if ( action )
  {
    QgsGui::mapLayerActionRegistry()->removeMapLayerAction( action );
    delete action;
    action = nullptr;
    mAtlasFeatureActions.remove( layout );
  }

  if ( enableAction )
  {
    action = new QgsMapLayerAction( tr( "Set as Atlas Feature for %1" ).arg( layout->name() ),
                                    this, layout->atlas()->coverageLayer(), QgsMapLayerAction::SingleFeature,
                                    QgsApplication::getThemeIcon( QStringLiteral( "/mIconAtlas.svg" ) ) );
    mAtlasFeatureActions.insert( layout, action );
    QgsGui::mapLayerActionRegistry()->addMapLayerAction( action );
    connect( action, &QgsMapLayerAction::triggeredForFeature, this, [this, layout]( QgsMapLayer * layer, const QgsFeature & feat )
    {
      Q_UNUSED( layer )
      setLayoutAtlasFeature( layout, feat );
    }
           );
  }
}

void QgisApp::setLayoutAtlasFeature( QgsPrintLayout *layout, const QgsFeature &feat )
{
  QgsLayoutDesignerDialog *designer = openLayoutDesignerDialog( layout );
  designer->setAtlasFeature( feat );
}

void QgisApp::layoutsMenuAboutToShow()
{
  populateLayoutsMenu( mLayoutsMenu );
}

void QgisApp::populateLayoutsMenu( QMenu *menu )
{
  menu->clear();
  QList<QAction *> acts;
  const QList< QgsMasterLayoutInterface * > layouts = QgsProject::instance()->layoutManager()->layouts();
  acts.reserve( layouts.size() );
  for ( QgsMasterLayoutInterface *layout : layouts )
  {
    QAction *a = new QAction( layout->name(), menu );
    connect( a, &QAction::triggered, this, [this, layout]
    {
      openLayoutDesignerDialog( layout );
    } );
    acts << a;
  }
  if ( acts.size() > 1 )
  {
    // sort actions by text
    std::sort( acts.begin(), acts.end(), cmpByText_ );
  }
  menu->addActions( acts );
}

void QgisApp::populate3DMapviewsMenu( QMenu *menu )
{
#ifdef HAVE_3D
  menu->clear();
  QList<QAction *> acts;
  QList< QDomElement > views = QgsProject::instance()->viewsManager()->get3DViews();
  acts.reserve( views.size() );
  for ( const QDomElement &viewConfig : views )
  {
    QString viewName = viewConfig.attribute( QStringLiteral( "name" ) );
    bool isOpen = viewConfig.attribute( QStringLiteral( "isOpen" ), QStringLiteral( "1" ) ).toInt() == 1;
    QAction *a = new QAction( viewName, menu );
    a->setCheckable( true );
    a->setChecked( isOpen );
    connect( a, &QAction::triggered, this, [a]( bool isChecked )
    {
      QString viewName = a->text();
      if ( isChecked )
      {
        QgisApp::instance()->open3DMapView( viewName );
      }
      else
      {
        QgisApp::instance()->close3DMapView( viewName );
      }
    } );
    acts << a;
  }
  if ( acts.size() > 1 )
  {
    // sort actions by text
    std::sort( acts.begin(), acts.end(), cmpByText_ );
  }
  menu->addActions( acts );
  menu->addSeparator();
  menu->addAction( mActionNew3DMapCanvas );
  menu->addAction( mActionManage3DMapViews );

#else
  Q_UNUSED( menu );
#endif
}

void QgisApp::views3DMenuAboutToShow()
{
  populate3DMapviewsMenu( m3DMapViewsMenu );
}

void QgisApp::showPinnedLabels( bool show )
{
  mMapTools->mapTool< QgsMapToolPinLabels >( QgsAppMapTools::PinLabels )->showPinnedLabels( show );
}

void QgisApp::pinLabels()
{
  mActionShowPinnedLabels->setChecked( true );
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::PinLabels ) );
}

void QgisApp::showHideLabels()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::ShowHideLabels ) );
}

void QgisApp::moveLabel()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::MoveLabel ) );
}

void QgisApp::rotateFeature()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::RotateFeature ) );
}

void QgisApp::scaleFeature()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::ScaleFeature ) );
}

void QgisApp::rotateLabel()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::RotateLabel ) );
}

void QgisApp::changeLabelProperties()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::ChangeLabelProperties ) );
}

QList<QgsMapCanvasAnnotationItem *> QgisApp::annotationItems()
{
  QList<QgsMapCanvasAnnotationItem *> itemList;

  if ( !mMapCanvas )
  {
    return itemList;
  }

  if ( mMapCanvas )
  {
    QList<QGraphicsItem *> graphicsItems = mMapCanvas->items();
    QList<QGraphicsItem *>::iterator gIt = graphicsItems.begin();
    for ( ; gIt != graphicsItems.end(); ++gIt )
    {
      QgsMapCanvasAnnotationItem *currentItem = dynamic_cast<QgsMapCanvasAnnotationItem *>( *gIt );
      if ( currentItem )
      {
        itemList.push_back( currentItem );
      }
    }
  }
  return itemList;
}

QList<QgsMapCanvas *> QgisApp::mapCanvases()
{
  // filter out browser canvases -- they are children of app, but a different
  // kind of beast, and here we only want the main canvas or dock canvases
  auto canvases = findChildren< QgsMapCanvas * >();
  canvases.erase( std::remove_if( canvases.begin(), canvases.end(),
                                  []( QgsMapCanvas * canvas )
  {
    return !canvas || canvas->property( "browser_canvas" ).toBool();
  } ), canvases.end() );
  return canvases;
}

void QgisApp::removeAnnotationItems()
{
  if ( !mMapCanvas )
  {
    return;
  }
  QGraphicsScene *scene = mMapCanvas->scene();
  if ( !scene )
  {
    return;
  }
  QList<QgsMapCanvasAnnotationItem *> itemList = annotationItems();
  const auto constItemList = itemList;
  for ( QgsMapCanvasAnnotationItem *item : constItemList )
  {
    if ( item )
    {
      scene->removeItem( item );
      delete item;
    }
  }
}

void QgisApp::mergeAttributesOfSelectedFeatures()
{
  //get active layer (hopefully vector)
  QgsMapLayer *activeMapLayer = activeLayer();
  if ( !activeMapLayer )
  {
    visibleMessageBar()->pushMessage( tr( "No active layer" ),
                                      tr( "No active layer found. Please select a layer in the layer list" ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( activeMapLayer );
  if ( !vl )
  {
    visibleMessageBar()->pushMessage(
      tr( "Layer not editable" ),
      tr( "The merge features tool only works on vector layers." ),
      Qgis::MessageLevel::Warning );
    return;
  }

  if ( !vl->isEditable() )
  {
    visibleMessageBar()->pushMessage(
      tr( "Layer not editable" ),
      tr( "Merging features can only be done for layers in editing mode." ),
      Qgis::MessageLevel::Warning );

    return;
  }

  //get selected feature ids (as a QSet<int> )
  const QgsFeatureIds &featureIdSet = vl->selectedFeatureIds();
  if ( featureIdSet.size() < 2 )
  {
    visibleMessageBar()->pushMessage(
      tr( "Not enough features selected" ),
      tr( "The merge tool requires at least two selected features." ),
      Qgis::MessageLevel::Warning );
    return;
  }

  //get initial selection (may be altered by attribute merge dialog later)
  QgsFeatureList featureList = vl->selectedFeatures();

  //merge the attributes together
  QgsMergeAttributesDialog d( featureList, vl, mapCanvas() );
  //initialize dialog with all columns set to skip
  d.setAllToSkip();
  if ( d.exec() == QDialog::Rejected )
  {
    return;
  }

  vl->beginEditCommand( tr( "Merged feature attributes" ) );

  QgsAttributes merged = d.mergedAttributes();
  QSet<int> toSkip = d.skippedAttributeIndexes();

  bool firstFeature = true;
  const auto constSelectedFeatureIds = vl->selectedFeatureIds();
  for ( QgsFeatureId fid : constSelectedFeatureIds )
  {
    for ( int i = 0; i < merged.count(); ++i )
    {
      if ( toSkip.contains( i ) )
        continue;

      QVariant val = merged.at( i );
      QgsField fld( vl->fields().at( i ) );
      bool isDefaultValue = vl->fields().fieldOrigin( i ) == QgsFields::OriginProvider &&
                            vl->dataProvider() &&
                            vl->dataProvider()->defaultValueClause( vl->fields().fieldOriginIndex( i ) ) == val;

      // convert to destination data type
      QString errorMessage;
      if ( !isDefaultValue && !fld.convertCompatible( val, &errorMessage ) )
      {
        if ( firstFeature )
        {
          //only warn on first feature
          visibleMessageBar()->pushMessage(
            tr( "Invalid result" ),
            tr( "Could not store value '%1' in field of type %2: %3" ).arg( merged.at( i ).toString(), fld.typeName(), errorMessage ),
            Qgis::MessageLevel::Warning );
        }
      }
      else
      {
        vl->changeAttributeValue( fid, i, val );
      }
    }
    firstFeature = false;
  }

  vl->endEditCommand();

  vl->triggerRepaint();
}

void QgisApp::modifyAttributesOfSelectedFeatures()
{
  QgsMapLayer *activeMapLayer = activeLayer();
  if ( !activeMapLayer )
  {
    visibleMessageBar()->pushMessage(
      tr( "No active layer" ),
      tr( "Please select a layer in the layer list" ),
      Qgis::MessageLevel::Warning );
    return;
  }

  QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( activeMapLayer );
  if ( !vl )
  {
    visibleMessageBar()->pushMessage(
      tr( "Invalid layer" ),
      tr( "The merge features tool only works on vector layers." ),
      Qgis::MessageLevel::Warning );
    return;
  }
  if ( !vl->isEditable() )
  {
    visibleMessageBar()->pushMessage(
      tr( "Layer not editable" ),
      tr( "Modifying features can only be done for layers in editing mode." ),
      Qgis::MessageLevel::Warning );

    return;
  }

  QgsAttributeEditorContext context( createAttributeEditorContext() );
  context.setAllowCustomUi( false );
  context.setVectorLayerTools( mVectorLayerTools );
  context.setCadDockWidget( mAdvancedDigitizingDockWidget );
  context.setMapCanvas( mMapCanvas );

  QgsAttributeDialog *dialog = nullptr;
  if ( vl->selectedFeatureCount() == 1 )
  {
    context.setAttributeFormMode( QgsAttributeEditorContext::Mode::SingleEditMode );
    QgsFeature f = vl->selectedFeatures().at( 0 );
    dialog = new QgsAttributeDialog( vl, &f, false, this, true, context );
    dialog->setMode( QgsAttributeEditorContext::SingleEditMode );
  }
  else
  {
    context.setAttributeFormMode( QgsAttributeEditorContext::Mode::MultiEditMode );

    //dummy feature
    QgsFeature f( vl->fields() );
    dialog = new QgsAttributeDialog( vl, &f, false, this, true, context );
    dialog->setMode( QgsAttributeEditorContext::MultiEditMode );
  }
  dialog->setAttribute( Qt::WA_DeleteOnClose );
  dialog->show();
}

void QgisApp::mergeSelectedFeatures()
{
  //get active layer (hopefully vector)
  QgsMapLayer *activeMapLayer = activeLayer();
  if ( !activeMapLayer )
  {
    visibleMessageBar()->pushMessage(
      tr( "No active layer" ),
      tr( "Please select a layer in the layer list" ),
      Qgis::MessageLevel::Warning );
    return;
  }
  QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( activeMapLayer );
  if ( !vl )
  {
    visibleMessageBar()->pushMessage(
      tr( "Invalid layer" ),
      tr( "The merge features tool only works on vector layers." ),
      Qgis::MessageLevel::Warning );
    return;
  }
  if ( !vl->isEditable() )
  {
    visibleMessageBar()->pushMessage(
      tr( "Layer not editable" ),
      tr( "Merging features can only be done for layers in editing mode." ),
      Qgis::MessageLevel::Warning );

    return;
  }

  //get selected feature ids (as a QSet<int> )
  const QgsFeatureIds &featureIdSet = vl->selectedFeatureIds();
  if ( featureIdSet.size() < 2 )
  {
    visibleMessageBar()->pushMessage(
      tr( "Not enough features selected" ),
      tr( "The merge tool requires at least two selected features" ),
      Qgis::MessageLevel::Warning );
    return;
  }

  //get initial selection (may be altered by attribute merge dialog later)
  QgsFeatureIds featureIds = vl->selectedFeatureIds();
  QgsFeatureList featureList = vl->selectedFeatures();
  bool canceled;
  QgsGeometry unionGeom = unionGeometries( vl, featureList, canceled );
  if ( unionGeom.isNull() )
  {
    if ( !canceled )
    {
      visibleMessageBar()->pushMessage(
        tr( "Merge failed" ),
        tr( "An error occurred during the merge operation." ),
        Qgis::MessageLevel::Critical );
    }
    return;
  }
  else if ( !QgsWkbTypes::isMultiType( vl->wkbType() ) )
  {
    const QgsGeometryCollection *c = qgsgeometry_cast<const QgsGeometryCollection *>( unionGeom.constGet() );
    if ( ( c && c->partCount() > 1 ) || !unionGeom.convertToSingleType() )
    {
      visibleMessageBar()->pushMessage(
        tr( "Merge failed" ),
        tr( "Resulting geometry type (multipart) is incompatible with layer type (singlepart)." ),
        Qgis::MessageLevel::Critical );
      return;
    }
  }

  //merge the attributes together
  QgsMergeAttributesDialog d( featureList, vl, mapCanvas() );
  d.setWindowTitle( tr( "Merge Features" ) );
  if ( d.exec() == QDialog::Rejected )
  {
    return;
  }

  QgsFeatureIds featureIdsAfter = vl->selectedFeatureIds();

  if ( featureIdsAfter.size() < 2 )
  {
    visibleMessageBar()->pushMessage(
      tr( "Not enough features selected" ),
      tr( "The merge tool requires at least two selected features" ),
      Qgis::MessageLevel::Warning );
    return;
  }

  //if the user changed the feature selection in the merge dialog, we need to repeat the union and check the type
  if ( featureIds.size() != featureIdsAfter.size() )
  {
    bool canceled;
    QgsFeatureList featureListAfter = vl->selectedFeatures();
    unionGeom = unionGeometries( vl, featureListAfter, canceled );
    if ( unionGeom.isNull() )
    {
      if ( !canceled )
      {
        visibleMessageBar()->pushMessage(
          tr( "Merge failed" ),
          tr( "An error occurred during the merge operation." ),
          Qgis::MessageLevel::Critical );
      }
      return;
    }
  }

  QgsAttributes attrs = d.mergedAttributes();
  QgsAttributeMap newAttributes;
  QString errorMessage;
  QgsFeatureId mergeFeatureId = FID_NULL;
  for ( int i = 0; i < attrs.count(); ++i )
  {
    QVariant val = attrs.at( i );
    bool isDefaultValue = vl->fields().fieldOrigin( i ) == QgsFields::OriginProvider &&
                          vl->dataProvider() &&
                          vl->dataProvider()->defaultValueClause( vl->fields().fieldOriginIndex( i ) ) == val;
    bool isPrimaryKey =  vl->fields().fieldOrigin( i ) == QgsFields::OriginProvider &&
                         vl->dataProvider() &&
                         vl->dataProvider()->pkAttributeIndexes().contains( vl->fields().fieldOriginIndex( i ) );

    if ( isPrimaryKey && !isDefaultValue )
      mergeFeatureId = val.toLongLong();

    // convert to destination data type
    if ( !isDefaultValue && !vl->fields().at( i ).convertCompatible( val, &errorMessage ) )
    {
      visibleMessageBar()->pushMessage(
        tr( "Invalid result" ),
        tr( "Could not store value '%1' in field of type %2: %3" ).arg( attrs.at( i ).toString(), vl->fields().at( i ).typeName(), errorMessage ),
        Qgis::MessageLevel::Warning );
    }
    newAttributes[ i ] = val;
  }

  vl->beginEditCommand( tr( "Merged features" ) );

  QgsFeature mergeFeature;
  if ( mergeFeatureId == FID_NULL )
  {
    // Create new feature
    mergeFeature = QgsVectorLayerUtils::createFeature( vl, unionGeom, newAttributes );
  }
  else
  {
    // Merge into existing feature
    featureIdsAfter.remove( mergeFeatureId );
  }

  // Delete other features
  QgsFeatureIds::const_iterator feature_it = featureIdsAfter.constBegin();
  for ( ; feature_it != featureIdsAfter.constEnd(); ++feature_it )
  {
    vl->deleteFeature( *feature_it );
  }


  if ( mergeFeatureId == FID_NULL )
  {
    // Add the new feature
    vl->addFeature( mergeFeature );
  }
  else
  {
    // Modify merge feature
    vl->changeGeometry( mergeFeatureId, unionGeom );
    vl->changeAttributeValues( mergeFeatureId, newAttributes );
  }

  vl->endEditCommand();

  vl->triggerRepaint();
}

void QgisApp::vertexTool()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::VertexTool ) );
}

void QgisApp::vertexToolActiveLayer()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::VertexToolActiveLayer ) );
}

void QgisApp::rotatePointSymbols()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::RotatePointSymbolsTool ) );
}

void QgisApp::offsetPointSymbol()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::OffsetPointSymbolTool ) );
}

void QgisApp::snappingOptions()
{
  mSnappingDialogContainer->show();
}

void QgisApp::splitFeatures()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::SplitFeatures ) );
}

void QgisApp::splitParts()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::SplitParts ) );
}

void QgisApp::reshapeFeatures()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::ReshapeFeatures ) );
}

void QgisApp::addFeature()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::AddFeature ) );
}

void QgisApp::setMapTool( QgsMapTool *tool, bool clean )
{
  mMapCanvas->setMapTool( tool, clean );
}

void QgisApp::selectFeatures()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::SelectFeatures ) );
}

void QgisApp::selectByPolygon()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::SelectPolygon ) );
}

void QgisApp::selectByFreehand()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::SelectFreehand ) );
}

void QgisApp::selectByRadius()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::SelectRadius ) );
}

void QgisApp::deselectAll()
{
  // Turn off rendering to improve speed.
  QgsCanvasRefreshBlocker refreshBlocker;

  QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( QMap<QString, QgsMapLayer *>::iterator it = layers.begin(); it != layers.end(); ++it )
  {
    QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( !vl )
      continue;

    vl->removeSelection();
  }
}

void QgisApp::deselectActiveLayer()
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mMapCanvas->currentLayer() );

  if ( !vlayer )
  {
    visibleMessageBar()->pushMessage(
      tr( "No active vector layer" ),
      tr( "To deselect all features, choose a vector layer in the legend" ),
      Qgis::MessageLevel::Info );
    return;
  }

  vlayer->removeSelection();
}

void QgisApp::invertSelection()
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mMapCanvas->currentLayer() );
  if ( !vlayer )
  {
    visibleMessageBar()->pushMessage(
      tr( "No active vector layer" ),
      tr( "To invert selection, choose a vector layer in the legend" ),
      Qgis::MessageLevel::Info );
    return;
  }

  vlayer->invertSelection();
}

void QgisApp::selectAll()
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mMapCanvas->currentLayer() );
  if ( !vlayer )
  {
    visibleMessageBar()->pushMessage(
      tr( "No active vector layer" ),
      tr( "To select all, choose a vector layer in the legend." ),
      Qgis::MessageLevel::Info );
    return;
  }

  vlayer->selectAll();
}

void QgisApp::selectByExpression()
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mMapCanvas->currentLayer() );
  if ( !vlayer )
  {
    visibleMessageBar()->pushMessage(
      tr( "No active vector layer" ),
      tr( "To select features, choose a vector layer in the legend." ),
      Qgis::MessageLevel::Info );
    return;
  }

  QgsExpressionSelectionDialog *dlg = new QgsExpressionSelectionDialog( vlayer, QString(), this );
  dlg->setMessageBar( messageBar() );
  dlg->setMapCanvas( mapCanvas() );
  dlg->setAttribute( Qt::WA_DeleteOnClose );
  dlg->show();
}

void QgisApp::selectByForm()
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mMapCanvas->currentLayer() );
  if ( !vlayer )
  {
    visibleMessageBar()->pushMessage(
      tr( "No active vector layer" ),
      tr( "To select features, choose a vector layer in the legend." ),
      Qgis::MessageLevel::Info );
    return;
  }
  QgsDistanceArea myDa;

  myDa.setSourceCrs( vlayer->crs(), QgsProject::instance()->transformContext() );
  myDa.setEllipsoid( QgsProject::instance()->ellipsoid() );

  QgsAttributeEditorContext context;
  context.setDistanceArea( myDa );
  context.setVectorLayerTools( mVectorLayerTools );
  context.setCadDockWidget( mAdvancedDigitizingDockWidget );
  context.setMapCanvas( mMapCanvas );

  QgsSelectByFormDialog *dlg = new QgsSelectByFormDialog( vlayer, context, this );
  dlg->setMessageBar( messageBar() );
  dlg->setMapCanvas( mapCanvas() );
  dlg->setAttribute( Qt::WA_DeleteOnClose );
  connect( dlg, &QgsSelectByFormDialog::showFilteredFeaturesAttributeTable, [ = ]( const QString & filter )
  {
    if ( !vlayer->dataProvider() )
    {
      return;
    }

    QgsAttributeTableDialog *dialog = new QgsAttributeTableDialog( vlayer, QgsAttributeTableFilterModel::FilterMode::ShowFilteredList );
    dialog->setFilterExpression( filter );
    dialog->setView( QgsDualView::ViewMode::AttributeEditor );
    dialog->show();

  } );
  dlg->show();
}

void QgisApp::addRing()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::AddRing ) );
}

void QgisApp::fillRing()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::FillRing ) );
}


void QgisApp::addPart()
{
  mMapCanvas->setMapTool( mMapTools->mapTool( QgsAppMapTools::AddPart ) );
}


void QgisApp::cutSelectionToClipboard( QgsMapLayer *layerContainingSelection )
{
  // Test for feature support in this layer
  QgsVectorLayer *selectionVectorLayer = qobject_cast<QgsVectorLayer *>( layerContainingSelection ? layerContainingSelection : activeLayer() );
  if ( !selectionVectorLayer )
    return;

  if ( !selectionVectorLayer->isEditable() )
  {
    visibleMessageBar()->pushMessage( tr( "Layer not editable" ),
                                      tr( "The current layer is not editable. Choose 'Start editing' in the digitizing toolbar." ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  clipboard()->replaceWithCopyOf( selectionVectorLayer );

  selectionVectorLayer->beginEditCommand( tr( "Features cut" ) );
  selectionVectorLayer->deleteSelectedFeatures();
  selectionVectorLayer->endEditCommand();
}

void QgisApp::copySelectionToClipboard( QgsMapLayer *layerContainingSelection )
{
  QgsVectorLayer *selectionVectorLayer = qobject_cast<QgsVectorLayer *>( layerContainingSelection ? layerContainingSelection : activeLayer() );
  if ( !selectionVectorLayer )
    return;

  // Test for feature support in this layer
  clipboard()->replaceWithCopyOf( selectionVectorLayer );
}

void QgisApp::clipboardChanged()
{
  activateDeactivateLayerRelatedActions( activeLayer() );
}

void QgisApp::pasteFromClipboard( QgsMapLayer *destinationLayer )
{
  QgsVectorLayer *pasteVectorLayer = qobject_cast<QgsVectorLayer *>( destinationLayer ? destinationLayer : activeLayer() );
  if ( !pasteVectorLayer )
    return;

  if ( !pasteVectorLayer->isEditable() )
  {
    visibleMessageBar()->pushMessage( tr( "Layer not editable" ),
                                      tr( "The current layer is not editable. Choose 'Start editing' in the digitizing toolbar." ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  pasteVectorLayer->beginEditCommand( tr( "Features pasted" ) );
  QgsFeatureList features = clipboard()->transformedCopyOf( pasteVectorLayer->crs(), pasteVectorLayer->fields() );
  int nTotalFeatures = features.count();
  QgsExpressionContext context = pasteVectorLayer->createExpressionContext();

  QgsFeatureList compatibleFeatures( QgsVectorLayerUtils::makeFeaturesCompatible( features, pasteVectorLayer, QgsFeatureSink::RegeneratePrimaryKey ) );
  QgsVectorLayerUtils::QgsFeaturesDataList newFeaturesDataList;
  newFeaturesDataList.reserve( compatibleFeatures.size() );

  // Count collapsed geometries
  int invalidGeometriesCount = 0;

  for ( const auto &feature : std::as_const( compatibleFeatures ) )
  {

    QgsGeometry geom = feature.geometry();

    if ( !( geom.isEmpty() || geom.isNull( ) ) )
    {
      // avoid intersection if enabled in digitize settings
      QList<QgsVectorLayer *>  avoidIntersectionsLayers;
      switch ( QgsProject::instance()->avoidIntersectionsMode() )
      {
        case QgsProject::AvoidIntersectionsMode::AvoidIntersectionsCurrentLayer:
          avoidIntersectionsLayers.append( pasteVectorLayer );
          break;
        case QgsProject::AvoidIntersectionsMode::AvoidIntersectionsLayers:
          avoidIntersectionsLayers = QgsProject::instance()->avoidIntersectionsLayers();
          break;
        case QgsProject::AvoidIntersectionsMode::AllowIntersections:
          break;
      }
      if ( avoidIntersectionsLayers.size() > 0 )
      {
        geom.avoidIntersections( avoidIntersectionsLayers );
      }

      // count collapsed geometries
      if ( geom.isEmpty() || geom.isNull( ) )
        invalidGeometriesCount++;
    }

    QgsAttributeMap attrMap;
    for ( int i = 0; i < feature.attributes().count(); i++ )
    {
      attrMap[i] = feature.attribute( i );
    }
    newFeaturesDataList << QgsVectorLayerUtils::QgsFeatureData( geom, attrMap );
  }

  // now create new feature using pasted feature as a template. This automatically handles default
  // values and field constraints
  QgsFeatureList newFeatures {QgsVectorLayerUtils::createFeatures( pasteVectorLayer, newFeaturesDataList, &context )};

  // check constraints
  bool hasStrongConstraints = false;

  for ( const QgsField &field : pasteVectorLayer->fields() )
  {
    if ( ( field.constraints().constraints() & QgsFieldConstraints::ConstraintUnique && field.constraints().constraintStrength( QgsFieldConstraints::ConstraintUnique ) & QgsFieldConstraints::ConstraintStrengthHard )
         || ( field.constraints().constraints() & QgsFieldConstraints::ConstraintNotNull && field.constraints().constraintStrength( QgsFieldConstraints::ConstraintNotNull ) & QgsFieldConstraints::ConstraintStrengthHard )
         || ( field.constraints().constraints() & QgsFieldConstraints::ConstraintExpression && !field.constraints().constraintExpression().isEmpty() && field.constraints().constraintStrength( QgsFieldConstraints::ConstraintExpression ) & QgsFieldConstraints::ConstraintStrengthHard )
       )
    {
      hasStrongConstraints = true;
      break;
    }
  }

  if ( hasStrongConstraints )
  {
    QgsFeatureList validFeatures = newFeatures;
    QgsFeatureList invalidFeatures;
    QMutableListIterator<QgsFeature> it( validFeatures );
    while ( it.hasNext() )
    {
      QgsFeature &f = it.next();
      for ( int idx = 0; idx < pasteVectorLayer->fields().count(); ++idx )
      {
        QStringList errors;
        if ( !QgsVectorLayerUtils::validateAttribute( pasteVectorLayer, f, idx, errors, QgsFieldConstraints::ConstraintStrengthHard, QgsFieldConstraints::ConstraintOriginNotSet ) )
        {
          invalidFeatures << f;
          it.remove();
          break;
        }
      }
    }

    if ( !invalidFeatures.isEmpty() )
    {
      newFeatures.clear();

      QgsAttributeEditorContext context( createAttributeEditorContext() );
      context.setAllowCustomUi( false );
      context.setFormMode( QgsAttributeEditorContext::StandaloneDialog );

      QgsFixAttributeDialog *dialog = new QgsFixAttributeDialog( pasteVectorLayer, invalidFeatures, this, context );

      connect( dialog, &QgsFixAttributeDialog::finished, this, [ = ]( int feedback )
      {
        QgsFeatureList features = newFeatures;
        switch ( feedback )
        {
          case QgsFixAttributeDialog::PasteValid:
            //paste valid and fixed, vanish unfixed
            features << validFeatures << dialog->fixedFeatures();
            break;
          case QgsFixAttributeDialog::PasteAll:
            //paste all, even unfixed
            features << validFeatures << dialog->fixedFeatures() << dialog->unfixedFeatures();
            break;
        }
        pasteFeatures( pasteVectorLayer, invalidGeometriesCount, nTotalFeatures, features );
        dialog->deleteLater();
      } );
      dialog->show();
      return;
    }
  }

  pasteFeatures( pasteVectorLayer, invalidGeometriesCount, nTotalFeatures, newFeatures );
}

void QgisApp::pasteFeatures( QgsVectorLayer *pasteVectorLayer, int invalidGeometriesCount, int nTotalFeatures, QgsFeatureList &features )
{
  int nCopiedFeatures = features.count();
  if ( pasteVectorLayer->addFeatures( features ) )
  {
    QgsFeatureIds newIds;
    newIds.reserve( features.size() );
    for ( const QgsFeature &f : std::as_const( features ) )
    {
      newIds << f.id();
    }

    pasteVectorLayer->selectByIds( newIds );
  }
  else
  {
    nCopiedFeatures = 0;
  }
  pasteVectorLayer->endEditCommand();
  pasteVectorLayer->updateExtents();

  Qgis::MessageLevel level = ( nCopiedFeatures == 0 || invalidGeometriesCount > 0 ) ? Qgis::MessageLevel::Warning : Qgis::MessageLevel::Info;
  QString message;
  if ( nCopiedFeatures == 0 )
  {
    message = tr( "No features pasted." );
  }
  else if ( nCopiedFeatures == nTotalFeatures )
  {
    message = tr( "%n feature(s) were pasted.", nullptr, nCopiedFeatures );
  }
  else
  {
    message = tr( "%1 of %2 features could be pasted." ).arg( nCopiedFeatures ).arg( nTotalFeatures );
  }

  // warn the user if the pasted features have invalid geometries
  if ( invalidGeometriesCount > 0 )
    message +=  invalidGeometriesCount == 1 ? tr( " Geometry collapsed due to intersection avoidance." ) :
                tr( "%n geometries collapsed due to intersection avoidance.", nullptr, invalidGeometriesCount );

  visibleMessageBar()->pushMessage( tr( "Paste features" ),
                                    message,
                                    level );

  pasteVectorLayer->triggerRepaint();
}

void QgisApp::pasteAsNewVector()
{

  std::unique_ptr< QgsVectorLayer > layer = pasteToNewMemoryVector();
  if ( !layer )
    return;

  saveAsVectorFileGeneral( layer.get(), false );
}

QgsVectorLayer *QgisApp::pasteAsNewMemoryVector( const QString &layerName )
{
  QString layerNameCopy = layerName;

  if ( layerNameCopy.isEmpty() )
  {
    bool ok;
    QString defaultName = tr( "Pasted" );
    layerNameCopy = QInputDialog::getText( this, tr( "Paste as Scratch Layer" ),
                                           tr( "Layer name" ), QLineEdit::Normal,
                                           defaultName, &ok );
    if ( !ok )
      return nullptr;

    if ( layerNameCopy.isEmpty() )
    {
      layerNameCopy = defaultName;
    }
  }

  std::unique_ptr< QgsVectorLayer > layer = pasteToNewMemoryVector();
  if ( !layer )
    return nullptr;

  layer->setName( layerNameCopy );

  QgsCanvasRefreshBlocker refreshBlocker;

  QgsVectorLayer *result = layer.get();
  QgsProject::instance()->addMapLayer( layer.release() );

  return result;
}

std::unique_ptr<QgsVectorLayer> QgisApp::pasteToNewMemoryVector()
{
  const QgsFields fields = clipboard()->fields();

  // Decide geometry type from features, switch to multi type if at least one multi is found
  QMap<QgsWkbTypes::Type, int> typeCounts;
  const QgsFeatureList features = clipboard()->copyOf( fields );
  for ( const QgsFeature &feature : features )
  {
    if ( !feature.hasGeometry() )
      continue;

    const QgsWkbTypes::Type type = feature.geometry().wkbType();

    if ( type == QgsWkbTypes::Unknown || type == QgsWkbTypes::NoGeometry )
      continue;

    if ( QgsWkbTypes::isSingleType( type ) )
    {
      if ( typeCounts.contains( QgsWkbTypes::multiType( type ) ) )
      {
        typeCounts[ QgsWkbTypes::multiType( type )] = typeCounts[ QgsWkbTypes::multiType( type )] + 1;
      }
      else
      {
        typeCounts[ type ] = typeCounts[ type ] + 1;
      }
    }
    else if ( QgsWkbTypes::isMultiType( type ) )
    {
      if ( typeCounts.contains( QgsWkbTypes::singleType( type ) ) )
      {
        // switch to multi type
        typeCounts[type] = typeCounts[ QgsWkbTypes::singleType( type )];
        typeCounts.remove( QgsWkbTypes::singleType( type ) );
      }
      typeCounts[type] = typeCounts[type] + 1;
    }
  }

  const QgsWkbTypes::Type wkbType = !typeCounts.isEmpty() ? typeCounts.keys().value( 0 ) : QgsWkbTypes::NoGeometry;

  if ( features.isEmpty() )
  {
    // should not happen
    visibleMessageBar()->pushMessage( tr( "Paste features" ),
                                      tr( "No features in clipboard." ),
                                      Qgis::MessageLevel::Info );
    return nullptr;
  }
  else if ( typeCounts.size() > 1 )
  {
    QString typeName = wkbType != QgsWkbTypes::NoGeometry ? QgsWkbTypes::displayString( wkbType ) : QStringLiteral( "none" );
    visibleMessageBar()->pushMessage( tr( "Paste features" ),
                                      tr( "Multiple geometry types found, features with geometry different from %1 will be created without geometry." ).arg( typeName ),
                                      Qgis::MessageLevel::Info );
  }

  std::unique_ptr< QgsVectorLayer > layer( QgsMemoryProviderUtils::createMemoryLayer( QStringLiteral( "pasted_features" ), QgsFields(), wkbType, clipboard()->crs() ) );

  if ( !layer->isValid() || !layer->dataProvider() )
  {
    visibleMessageBar()->pushMessage( tr( "Paste features" ),
                                      tr( "Cannot create new layer." ),
                                      Qgis::MessageLevel::Warning );
    return nullptr;
  }

  layer->startEditing();
  for ( const QgsField &f : clipboard()->fields() )
  {
    QgsDebugMsgLevel( QStringLiteral( "field %1 (%2)" ).arg( f.name(), QVariant::typeToName( f.type() ) ), 2 );
    if ( !layer->addAttribute( f ) )
    {
      visibleMessageBar()->pushMessage( tr( "Paste features" ),
                                        tr( "Cannot create field %1 (%2,%3)" ).arg( f.name(), f.typeName(), QVariant::typeToName( f.type() ) ),
                                        Qgis::MessageLevel::Warning );
      return nullptr;
    }
  }

  // Convert to multi if necessary
  QgsFeatureList convertedFeatures;
  convertedFeatures.reserve( features.length() );
  for ( QgsFeature feature : features )
  {
    if ( !feature.hasGeometry() )
    {
      convertedFeatures.append( feature );
      continue;
    }

    const QgsWkbTypes::Type type = feature.geometry().wkbType();
    if ( type == QgsWkbTypes::Unknown || type == QgsWkbTypes::NoGeometry )
    {
      convertedFeatures.append( feature );
      continue;
    }

    if ( QgsWkbTypes::singleType( wkbType ) != QgsWkbTypes::singleType( type ) )
    {
      feature.clearGeometry();
    }

    if ( QgsWkbTypes::isMultiType( wkbType ) &&  QgsWkbTypes::isSingleType( type ) )
    {
      QgsGeometry g = feature.geometry();
      g.convertToMultiType();
      feature.setGeometry( g );
    }
    convertedFeatures.append( feature );
  }
  if ( ! layer->addFeatures( convertedFeatures ) || !layer->commitChanges() )
  {
    QgsDebugMsg( QStringLiteral( "Cannot add features or commit changes" ) );
    return nullptr;
  }

  QgsDebugMsgLevel( QStringLiteral( "%1 features pasted to temporary scratch layer" ).arg( layer->featureCount() ), 2 );
  return layer;
}

void QgisApp::copyStyle( QgsMapLayer *sourceLayer, QgsMapLayer::StyleCategories categories )
{
  QgsMapLayer *selectionLayer = sourceLayer ? sourceLayer : activeLayer();

  if ( selectionLayer )
  {
    QString errorMsg;
    QDomDocument doc( QStringLiteral( "qgis" ) );
    QgsReadWriteContext context;
    selectionLayer->exportNamedStyle( doc, errorMsg, context, categories );

    if ( !errorMsg.isEmpty() )
    {
      visibleMessageBar()->pushMessage( tr( "Cannot copy style" ),
                                        errorMsg,
                                        Qgis::MessageLevel::Critical );
      return;
    }
    // Copies data in text form as well, so the XML can be pasted into a text editor
    clipboard()->setData( QStringLiteral( QGSCLIPBOARD_STYLE_MIME ), doc.toByteArray(), doc.toString() );

    // Enables the paste menu element
    mActionPasteStyle->setEnabled( true );
  }
}

void QgisApp::pasteStyle( QgsMapLayer *destinationLayer, QgsMapLayer::StyleCategories categories )
{
  QgsMapLayer *selectionLayer = destinationLayer ? destinationLayer : activeLayer();
  if ( selectionLayer )
  {
    if ( clipboard()->hasFormat( QStringLiteral( QGSCLIPBOARD_STYLE_MIME ) ) )
    {
      QDomDocument doc( QStringLiteral( "qgis" ) );
      QString errorMsg;
      int errorLine, errorColumn;
      if ( !doc.setContent( clipboard()->data( QStringLiteral( QGSCLIPBOARD_STYLE_MIME ) ), false, &errorMsg, &errorLine, &errorColumn ) )
      {

        visibleMessageBar()->pushMessage( tr( "Cannot parse style" ),
                                          errorMsg,
                                          Qgis::MessageLevel::Critical );
        return;
      }

      bool isVectorStyle = doc.elementsByTagName( QStringLiteral( "pipe" ) ).isEmpty();
      if ( ( selectionLayer->type() == QgsMapLayerType::RasterLayer && isVectorStyle ) ||
           ( selectionLayer->type() == QgsMapLayerType::VectorLayer && !isVectorStyle ) )
      {
        return;
      }

      if ( !selectionLayer->importNamedStyle( doc, errorMsg, categories ) )
      {
        visibleMessageBar()->pushMessage( tr( "Cannot paste style" ),
                                          errorMsg,
                                          Qgis::MessageLevel::Critical );
        return;
      }

      mLayerTreeView->refreshLayerSymbology( selectionLayer->id() );
      selectionLayer->triggerRepaint();
    }
  }
}

void QgisApp::copyLayer()
{
  QString errorMessage;
  QgsReadWriteContext readWriteContext;
  QDomDocument doc( QStringLiteral( "qgis-layer-definition" ) );

  bool saved = QgsLayerDefinition::exportLayerDefinition( doc, mLayerTreeView->selectedNodes(), errorMessage, readWriteContext );

  if ( !saved )
  {
    visibleMessageBar()->pushMessage( tr( "Error copying layer" ), errorMessage, Qgis::MessageLevel::Warning );
  }

  // Copies data in text form as well, so the XML can be pasted into a text editor
  clipboard()->setData( QStringLiteral( QGSCLIPBOARD_MAPLAYER_MIME ), doc.toByteArray(), doc.toString() );
  // Enables the paste menu element
  mActionPasteLayer->setEnabled( true );
}

void QgisApp::pasteLayer()
{
  if ( clipboard()->hasFormat( QStringLiteral( QGSCLIPBOARD_MAPLAYER_MIME ) ) )
  {
    QDomDocument doc;
    QString errorMessage;
    QgsReadWriteContext readWriteContext;
    doc.setContent( clipboard()->data( QStringLiteral( QGSCLIPBOARD_MAPLAYER_MIME ) ) );

    QgsLayerTreeNode *currentNode = mLayerTreeView->currentNode();
    QgsLayerTreeGroup *root = nullptr;
    if ( QgsLayerTree::isGroup( currentNode ) )
    {
      root = QgsLayerTree::toGroup( currentNode );
    }
    else
    {
      root = QgsProject::instance()->layerTreeRoot();
    }

    bool loaded = QgsLayerDefinition::loadLayerDefinition( doc, QgsProject::instance(), root,
                  errorMessage, readWriteContext );

    if ( !loaded || !errorMessage.isEmpty() )
    {
      visibleMessageBar()->pushMessage( tr( "Error pasting layer" ), errorMessage, Qgis::MessageLevel::Warning );
    }
  }
}

void QgisApp::copyFeatures( QgsFeatureStore &featureStore )
{
  clipboard()->replaceWithCopyOf( featureStore );
}

void QgisApp::refreshMapCanvas( bool redrawAllLayers )
{
  const auto canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
  {
    //stop any current rendering
    canvas->stopRendering();
    if ( redrawAllLayers )
      canvas->refreshAllLayers();
    else
      canvas->refresh();
  }
}

void QgisApp::canvasRefreshStarted()
{
  mLastRenderTime.restart();
  // if previous render took less than 0.5 seconds, delay the appearance of the
  // render in progress status bar by 0.5 seconds - this avoids the status bar
  // rapidly appearing and then disappearing for very fast renders
  if ( mLastRenderTimeSeconds > 0 && mLastRenderTimeSeconds < 0.5 )
  {
    mRenderProgressBarTimer.setSingleShot( true );
    mRenderProgressBarTimer.setInterval( 500 );
    disconnect( mRenderProgressBarTimerConnection );
    mRenderProgressBarTimerConnection = connect( &mRenderProgressBarTimer, &QTimer::timeout, this, [ = ]()
    {
      showProgress( -1, 0 );
    }
                                               );
    mRenderProgressBarTimer.start();
  }
  else
  {
    showProgress( -1, 0 ); // trick to make progress bar show busy indicator
  }
}

void QgisApp::canvasRefreshFinished()
{
  mRenderProgressBarTimer.stop();
  mLastRenderTimeSeconds = mLastRenderTime.elapsed() / 1000.0;
  showProgress( 0, 0 ); // stop the busy indicator
}

void QgisApp::toggleMapTips( bool enabled )
{
  mMapTipsVisible = enabled;
  // Store if maptips are active
  QgsSettings().setValue( QStringLiteral( "/qgis/enableMapTips" ), mMapTipsVisible );

  // if off, stop the timer
  if ( !mMapTipsVisible )
  {
    mpMapTipsTimer->stop();
    mpMaptip->clear( mMapCanvas );
  }

  if ( mActionMapTips->isChecked() != mMapTipsVisible )
    mActionMapTips->setChecked( mMapTipsVisible );
}

void QgisApp::toggleEditing()
{
  const QList<QgsMapLayer *> layerList = layerTreeView()->selectedLayers();
  if ( !layerList.isEmpty() )
  {
    // if there are selected layers, try to toggle those.
    // mActionToggleEditing has already been triggered at this point so its checked status has changed
    const bool shouldStartEditing = mActionToggleEditing->isChecked();
    for ( const auto layer : layerList )
    {
      if ( layer->supportsEditing() &&
           shouldStartEditing != layer->isEditable() )
      {
        toggleEditing( layer, true );
      }
    }
  }
  else
  {
    // if there are no selected layers, try to toggle the current layer
    QgsMapLayer *currentLayer =  activeLayer();
    if ( currentLayer && currentLayer->supportsEditing() )
    {
      toggleEditing( currentLayer, true );
    }
    else
    {
      // active although there's no layer active!?
      mActionToggleEditing->setChecked( false );
      mActionToggleEditing->setEnabled( false );
      visibleMessageBar()->pushMessage( tr( "Start editing failed" ),
                                        tr( "Layer cannot be edited" ),
                                        Qgis::MessageLevel::Warning );
    }
  }
}

bool QgisApp::toggleEditing( QgsMapLayer *layer, bool allowCancel )
{
  switch ( layer->type() )
  {
    case QgsMapLayerType::VectorLayer:
      return toggleEditingVectorLayer( qobject_cast<QgsVectorLayer *>( layer ), allowCancel );
    case QgsMapLayerType::MeshLayer:
      return toggleEditingMeshLayer( qobject_cast<QgsMeshLayer *>( layer ), allowCancel );
    case QgsMapLayerType::RasterLayer:
    case QgsMapLayerType::PluginLayer:
    case QgsMapLayerType::VectorTileLayer:
    case QgsMapLayerType::AnnotationLayer:
    case QgsMapLayerType::PointCloudLayer:
    case QgsMapLayerType::GroupLayer:
      break;
  }
  return false;
}

bool QgisApp::toggleEditingVectorLayer( QgsVectorLayer *vlayer, bool allowCancel )
{
  if ( !vlayer )
  {
    return false;
  }

  bool res = true;

  // Assume changes if: a) the layer reports modifications or b) its transaction group was modified
  QSet<QgsVectorLayer *> modifiedLayers;
  switch ( QgsProject::instance()->transactionMode() )
  {
    case Qgis::TransactionMode::Disabled:
    {
      if ( vlayer->isModified() )
        modifiedLayers.insert( vlayer );
    }
    break;
    case Qgis::TransactionMode::AutomaticGroups:
    {
      QString connString = QgsTransaction::connectionString( vlayer->source() );
      QString key = vlayer->providerType();

      QMap< QPair< QString, QString>, QgsTransactionGroup *> transactionGroups = QgsProject::instance()->transactionGroups();
      QMap< QPair< QString, QString>, QgsTransactionGroup *>::iterator tIt = transactionGroups .find( qMakePair( key, connString ) );
      QgsTransactionGroup *tg = ( tIt != transactionGroups.end() ? tIt.value() : nullptr );

      if ( tg && tg->layers().contains( vlayer ) && tg->modified() )
      {
        if ( vlayer->isModified() )
          modifiedLayers.insert( vlayer );
        const QSet<QgsVectorLayer *> transactionGroupLayers = tg->layers();
        for ( QgsVectorLayer *iterLayer : transactionGroupLayers )
        {
          if ( iterLayer != vlayer && iterLayer->isModified() )
            modifiedLayers.insert( iterLayer );
        }
      }
    }
    break;
    case Qgis::TransactionMode::BufferedGroups:
      modifiedLayers = QgsProject::instance()->editBufferGroup()->modifiedLayers();
      break;
  }


  if ( !vlayer->isEditable() && !vlayer->readOnly() )
  {
    if ( !vlayer->supportsEditing() )
    {
      mActionToggleEditing->setChecked( false );
      mActionToggleEditing->setEnabled( false );
      visibleMessageBar()->pushMessage( tr( "Start editing failed" ),
                                        tr( "Provider cannot be opened for editing" ),
                                        Qgis::MessageLevel::Warning );
      return false;
    }

    QgsProject::instance()->startEditing( vlayer );

    QString markerType = QgsSettingsRegistryCore::settingsDigitizingMarkerStyle.value();
    bool markSelectedOnly = QgsSettingsRegistryCore::settingsDigitizingMarkerOnlyForSelected.value();

    // redraw only if markers will be drawn
    if ( ( !markSelectedOnly || vlayer->selectedFeatureCount() > 0 ) &&
         ( markerType == QLatin1String( "Cross" ) || markerType == QLatin1String( "SemiTransparentCircle" ) ) )
    {
      vlayer->triggerRepaint();
    }
  }
  else if ( modifiedLayers.size() > 0 )
  {
    QMessageBox::StandardButtons buttons = QMessageBox::Save | QMessageBox::Discard;
    if ( allowCancel )
      buttons |= QMessageBox::Cancel;

    QString modifiedLayerNames;
    if ( modifiedLayers.size() == 1 )
      modifiedLayerNames = ( *modifiedLayers.constBegin() )->name();
    else if ( modifiedLayers.size() == 2 )
      modifiedLayerNames = tr( "%1 and %2" ).arg( ( *modifiedLayers.constBegin() )->name(), ( * ++modifiedLayers.constBegin() )->name() );
    else if ( modifiedLayers.size() > 2 )
      modifiedLayerNames = tr( "%1, %2, …" ).arg( ( *modifiedLayers.constBegin() )->name(), ( * ++modifiedLayers.constBegin() )->name() );

    switch ( QMessageBox::question( nullptr,
                                    tr( "Stop Editing" ),
                                    modifiedLayers.size() > 0 ?
                                    tr( "Do you want to save the changes to layers %1?" ).arg( modifiedLayerNames ) :
                                    tr( "Do you want to save the changes to layer %1?" ).arg( modifiedLayerNames ),
                                    buttons ) )
    {
      case QMessageBox::Cancel:
        res = false;
        break;

      case QMessageBox::Save:
      {
        QApplication::setOverrideCursor( Qt::WaitCursor );

        QStringList commitErrors;
        if ( !QgsProject::instance()->commitChanges( commitErrors, true, vlayer ) )
        {
          commitError( vlayer, commitErrors );
          // Leave the in-memory editing state alone,
          // to give the user a chance to enter different values
          // and try the commit again later
          res = false;
        }

        vlayer->triggerRepaint();

        QApplication::restoreOverrideCursor();
      }
      break;

      case QMessageBox::Discard:
      {
        QApplication::setOverrideCursor( Qt::WaitCursor );

        QgsCanvasRefreshBlocker refreshBlocker;

        QStringList rollBackErrors;
        if ( ! QgsProject::instance()->rollBack( rollBackErrors, true, vlayer ) )
        {
          visibleMessageBar()->pushMessage( tr( "Error" ),
                                            tr( "Problems during roll back: '%1'" ).arg( rollBackErrors.join( " / " ) ),
                                            Qgis::MessageLevel::Critical );
          res = false;
        }

        vlayer->triggerRepaint();

        QApplication::restoreOverrideCursor();
        break;
      }

      default:
        break;
    }
  }
  else //layer not modified
  {
    QgsCanvasRefreshBlocker refreshBlocker;

    QStringList rollBackErrors;
    QgsProject::instance()->rollBack( rollBackErrors, true, vlayer );

    res = true;
    vlayer->triggerRepaint();
  }

  if ( !res && vlayer == activeLayer() )
  {
    // while also called when layer sends editingStarted/editingStopped signals,
    // this ensures correct restoring of gui state if toggling was canceled
    // or layer commit/rollback functions failed
    activateDeactivateLayerRelatedActions( vlayer );
  }

  return res;
}

bool QgisApp::toggleEditingMeshLayer( QgsMeshLayer *mlayer, bool allowCancel )
{
  if ( !mlayer )
    return false;

  if ( !mlayer->supportsEditing() )
    return false;

  bool res = false;

  QgsCoordinateTransform transform( mlayer->crs(), mMapCanvas->mapSettings().destinationCrs(), QgsProject::instance() );

  if ( !mlayer->isEditable() )
  {
    QMessageBox *messageBox = new QMessageBox( QMessageBox::NoIcon, tr( "Start Mesh Frame Edit" ),
        tr( "Starting editing the frame of this mesh layer will remove all dataset groups.\n"
            "Alternatively, you can create a new mesh layer from that one." ), QMessageBox::Cancel );

    messageBox->addButton( tr( "Edit Current Mesh" ), QMessageBox::NoRole );
    QPushButton *editCopyButton = messageBox->addButton( tr( "Edit a Copy" ), QMessageBox::NoRole );
    messageBox->setDefaultButton( QMessageBox::Cancel );

    messageBox->exec();

    if ( messageBox->clickedButton() == messageBox->button( QMessageBox::Cancel ) )
    {
      mActionToggleEditing->setChecked( false );
      return false;
    }
    else if ( messageBox->clickedButton() == editCopyButton )
    {
      QgsNewMeshLayerDialog *newMeshDialog = new QgsNewMeshLayerDialog( this );
      newMeshDialog->setSourceMeshLayer( mlayer, true );
      if ( newMeshDialog->exec() )
        mlayer = newMeshDialog->newLayer();
      else
      {
        mActionToggleEditing->setChecked( false );
        return false;
      }
    }

    res = mlayer->startFrameEditing( transform );
    mActionToggleEditing->setChecked( res );

    if ( !res )
    {
      visibleMessageBar()->pushWarning(
        tr( "Mesh editing" ),
        tr( "Unable to start mesh editing for layer \"%1\"" ).arg( mlayer->name() ) );
    }
  }
  else if ( mlayer->isModified() )
  {
    QMessageBox::StandardButtons buttons = QMessageBox::Save | QMessageBox::Discard;
    if ( allowCancel )
      buttons = buttons | QMessageBox::Cancel;
    switch ( QMessageBox::question( nullptr,
                                    tr( "Stop Editing" ),
                                    tr( "Do you want to save the changes to layer %1?" ).arg( mlayer->name() ),
                                    buttons ) )
    {
      case QMessageBox::Cancel:
        res = false;
        break;

      case QMessageBox::Save:
      {
        QgsTemporaryCursorOverride waitCursor( Qt::WaitCursor );
        QgsCanvasRefreshBlocker refreshBlocker;
        if ( !mlayer->commitFrameEditing( transform, false ) )
        {
          visibleMessageBar()->pushWarning(
            tr( "Mesh editing" ),
            tr( "Unable to save editing for layer \"%1\"" ).arg( mlayer->name() ) );
          res = false;
        }

        mlayer->triggerRepaint();
      }
      break;
      case QMessageBox::Discard:
      {
        QgsTemporaryCursorOverride waitCursor( Qt::WaitCursor );
        QgsCanvasRefreshBlocker refreshBlocker;
        if ( !mlayer->rollBackFrameEditing( transform, false ) )
        {
          visibleMessageBar()->pushMessage( tr( "Error" ),
                                            tr( "Problems during roll back" ),
                                            Qgis::MessageLevel::Critical );
          res = false;
        }

        mlayer->triggerRepaint();
        break;
      }

      default:
        break;
    }
  }
  else //mesh layer not modified
  {
    QgsTemporaryCursorOverride waitCursor( Qt::WaitCursor );
    QgsCanvasRefreshBlocker refreshBlocker;
    mlayer->rollBackFrameEditing( transform, false );
    mlayer->triggerRepaint();
  }

  if ( !res && mlayer == activeLayer() )
  {
    // while also called when layer sends editingStarted/editingStopped signals,
    // this ensures correct restoring of gui state if toggling was canceled
    // or layer commit/rollback functions failed
    activateDeactivateLayerRelatedActions( mlayer );
  }

  return res;
}

void QgisApp::saveActiveLayerEdits()
{
  saveEdits( activeLayer(), true, true );
}

void QgisApp::saveEdits( QgsMapLayer *layer, bool leaveEditable, bool triggerRepaint )
{
  if ( !layer )
    return;

  switch ( layer->type() )
  {
    case QgsMapLayerType::VectorLayer:
      return saveVectorLayerEdits( layer, leaveEditable, triggerRepaint );
    case QgsMapLayerType::MeshLayer:
      return saveMeshLayerEdits( layer, leaveEditable, triggerRepaint );
    case QgsMapLayerType::RasterLayer:
    case QgsMapLayerType::PluginLayer:
    case QgsMapLayerType::VectorTileLayer:
    case QgsMapLayerType::AnnotationLayer:
    case QgsMapLayerType::PointCloudLayer:
    case QgsMapLayerType::GroupLayer:
      break;
  }
}

void QgisApp::saveVectorLayerEdits( QgsMapLayer *layer, bool leaveEditable, bool triggerRepaint )
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
  if ( !vlayer || !vlayer->isEditable() || !vlayer->isModified() )
    return;

  if ( vlayer == activeLayer() )
    mSaveRollbackInProgress = true;


  QStringList commitErrors;
  if ( !QgsProject::instance()->commitChanges( commitErrors, !leaveEditable, vlayer ) )
  {
    mSaveRollbackInProgress = false;
    commitError( vlayer, commitErrors );
  }

  if ( triggerRepaint )
  {
    vlayer->triggerRepaint();
  }
}

void QgisApp::saveMeshLayerEdits( QgsMapLayer *layer, bool leaveEditable, bool triggerRepaint )
{
  QgsMeshLayer *mlayer = qobject_cast<QgsMeshLayer *>( layer );
  if ( !mlayer || !mlayer->isEditable() || !mlayer->isModified() )
    return;

  if ( mlayer == activeLayer() )
    mSaveRollbackInProgress = true;

  QgsCanvasRefreshBlocker refreshBlocker;
  QgsCoordinateTransform transform( mlayer->crs(), mMapCanvas->mapSettings().destinationCrs(), QgsProject::instance() );

  if ( !mlayer->commitFrameEditing( transform, leaveEditable ) )
    visibleMessageBar()->pushWarning(
      tr( "Mesh editing" ),
      tr( "Unable to save editing for layer \"%1\"" ).arg( mlayer->name() ) );

  if ( triggerRepaint )
  {
    mlayer->triggerRepaint();
  }
}

void QgisApp::cancelEdits( QgsMapLayer *layer, bool leaveEditable, bool triggerRepaint )
{
  if ( !layer )
    return;

  switch ( layer->type() )
  {
    case QgsMapLayerType::VectorLayer:
      return cancelVectorLayerEdits( layer, leaveEditable, triggerRepaint );
    case QgsMapLayerType::MeshLayer:
      return cancelMeshLayerEdits( layer, leaveEditable, triggerRepaint );
    case QgsMapLayerType::RasterLayer:
    case QgsMapLayerType::PluginLayer:
    case QgsMapLayerType::VectorTileLayer:
    case QgsMapLayerType::AnnotationLayer:
    case QgsMapLayerType::PointCloudLayer:
    case QgsMapLayerType::GroupLayer:
      break;
  }
}

void QgisApp::cancelVectorLayerEdits( QgsMapLayer *layer, bool leaveEditable, bool triggerRepaint )
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
  if ( !vlayer || !vlayer->isEditable() )
    return;

  if ( vlayer == activeLayer() && leaveEditable )
    mSaveRollbackInProgress = true;

  QgsCanvasRefreshBlocker refreshBlocker;
  QStringList rollbackErrors;
  if ( ! QgsProject::instance()->rollBack( rollbackErrors, !leaveEditable, vlayer ) )
  {
    mSaveRollbackInProgress = false;
    QMessageBox::warning( nullptr,
                          tr( "Error" ),
                          tr( "Could not %1 changes to layer %2\n\nErrors: %3\n" )
                          .arg( leaveEditable ? tr( "rollback" ) : tr( "cancel" ),
                                vlayer->name(),
                                rollbackErrors.join( QLatin1String( "\n  " ) ) ) );
  }

  if ( leaveEditable )
  {
    vlayer->startEditing();
  }
  if ( triggerRepaint )
  {
    vlayer->triggerRepaint();
  }
}

void QgisApp::cancelMeshLayerEdits( QgsMapLayer *layer, bool leaveEditable, bool triggerRepaint )
{
  QgsMeshLayer *mlayer = qobject_cast<QgsMeshLayer *>( layer );
  if ( !mlayer || !mlayer->isEditable() )
    return;

  if ( mlayer == activeLayer() && leaveEditable )
    mSaveRollbackInProgress = true;

  QgsCanvasRefreshBlocker refreshBlocker;
  QgsCoordinateTransform transform( mlayer->crs(), mMapCanvas->mapSettings().destinationCrs(), QgsProject::instance() );
  if ( !mlayer->rollBackFrameEditing( transform, leaveEditable ) )
  {
    mSaveRollbackInProgress = false;
    QMessageBox::warning( nullptr,
                          tr( "Error" ),
                          tr( "Could not %1 changes to layer %2" )
                          .arg( leaveEditable ? tr( "rollback" ) : tr( "cancel" ),
                                mlayer->name() ) );
  }

  if ( triggerRepaint )
  {
    mlayer->triggerRepaint();
  }
}

void QgisApp::enableMeshEditingTools( bool enable )
{
  if ( !mMapTools )
    return;
  QgsMapToolEditMeshFrame *editMeshMapTool = qobject_cast<QgsMapToolEditMeshFrame *>( mMapTools->mapTool( QgsAppMapTools::EditMeshFrame ) );

  editMeshMapTool->setActionsEnable( enable );
}

QList<QgsMapToolCapture *> QgisApp::captureTools()
{
  QList< QgsMapToolCapture * > res = mMapTools->captureTools();
  // also check current tool, in case it's a custom tool
  if ( QgsMapToolCapture *currentTool = qobject_cast< QgsMapToolCapture * >( mMapCanvas->mapTool() ) )
  {
    if ( !res.contains( currentTool ) )
      res.append( currentTool );
  }
  return res;
}

void QgisApp::saveEdits()
{
  const auto constSelectedLayers = mLayerTreeView->selectedLayers();
  for ( QgsMapLayer *layer : constSelectedLayers )
  {
    saveEdits( layer, true, false );
  }
  refreshMapCanvas();
  activateDeactivateLayerRelatedActions( activeLayer() );
}

void QgisApp::saveAllEdits( bool verifyAction )
{
  if ( verifyAction )
  {
    if ( !verifyEditsActionDialog( tr( "Save" ), tr( "all" ) ) )
      return;
  }

  const auto layers = editableLayers( true, true );
  for ( QgsMapLayer *layer : layers )
  {
    saveEdits( layer, true, false );
  }
  refreshMapCanvas();
  activateDeactivateLayerRelatedActions( activeLayer() );
}

void QgisApp::rollbackEdits()
{
  const auto constSelectedLayers = mLayerTreeView->selectedLayers();
  for ( QgsMapLayer *layer : constSelectedLayers )
  {
    cancelEdits( layer, true, false );
  }
  refreshMapCanvas();
  activateDeactivateLayerRelatedActions( activeLayer() );
}

void QgisApp::rollbackAllEdits( bool verifyAction )
{
  if ( verifyAction )
  {
    if ( !verifyEditsActionDialog( tr( "Rollback" ), tr( "all" ) ) )
      return;
  }

  const auto layers = editableLayers( true, true );
  for ( QgsMapLayer *layer : layers )
  {
    cancelEdits( layer, true, false );
  }
  refreshMapCanvas();
  activateDeactivateLayerRelatedActions( activeLayer() );
}

void QgisApp::cancelEdits()
{
  const auto constSelectedLayers = mLayerTreeView->selectedLayers();
  for ( QgsMapLayer *layer : constSelectedLayers )
  {
    cancelEdits( layer, false, false );
  }
  refreshMapCanvas();
  activateDeactivateLayerRelatedActions( activeLayer() );
}

void QgisApp::cancelAllEdits( bool verifyAction )
{
  if ( verifyAction )
  {
    if ( !verifyEditsActionDialog( tr( "Cancel" ), tr( "all" ) ) )
      return;
  }

  const auto layers = editableLayers( false, true );
  for ( QgsMapLayer *layer : layers )
  {
    cancelEdits( layer, false, false );
  }
  refreshMapCanvas();
  activateDeactivateLayerRelatedActions( activeLayer() );
}

bool QgisApp::verifyEditsActionDialog( const QString &act, const QString &upon )
{
  bool res = false;
  switch ( QMessageBox::question( nullptr,
                                  tr( "Current edits" ),
                                  tr( "%1 current changes for %2 layer(s)?" )
                                  .arg( act,
                                        upon ),
                                  QMessageBox::Yes | QMessageBox::No ) )
  {
    case QMessageBox::Yes:
      res = true;
      break;
    default:
      break;
  }
  return res;
}

void QgisApp::updateLayerModifiedActions()
{
  bool enableSaveLayerEdits = false;

  QgsMapLayer *currentLayer = activeLayer();
  if ( currentLayer )
  {
    switch ( currentLayer->type() )
    {
      case QgsMapLayerType::VectorLayer:
      {
        QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( currentLayer );
        if ( QgsVectorDataProvider *dprovider = vlayer->dataProvider() )
        {
          enableSaveLayerEdits = ( dprovider->capabilities() & QgsVectorDataProvider::ChangeAttributeValues
                                   && vlayer->isEditable()
                                   && vlayer->isModified() );
        }
      }
      break;
      case QgsMapLayerType::MeshLayer:
      {
        QgsMeshLayer *mlayer = qobject_cast<QgsMeshLayer *>( currentLayer );
        enableSaveLayerEdits = ( mlayer->isEditable() && mlayer->isModified() );
      }
      break;
      case QgsMapLayerType::RasterLayer:
      case QgsMapLayerType::PluginLayer:
      case QgsMapLayerType::VectorTileLayer:
      case QgsMapLayerType::AnnotationLayer:
      case QgsMapLayerType::PointCloudLayer:
      case QgsMapLayerType::GroupLayer:
        break;
    }
  }

  mActionSaveLayerEdits->setEnabled( enableSaveLayerEdits );

  QList<QgsLayerTreeLayer *> selectedLayerNodes = mLayerTreeView ? mLayerTreeView->selectedLayerNodes() : QList<QgsLayerTreeLayer *>();

  mActionSaveEdits->setEnabled( QgsLayerTreeUtils::layersModified( selectedLayerNodes ) );
  mActionRollbackEdits->setEnabled( QgsLayerTreeUtils::layersModified( selectedLayerNodes ) );
  mActionCancelEdits->setEnabled( QgsLayerTreeUtils::layersEditable( selectedLayerNodes ) );

  bool hasEditLayers = !editableLayers( false, true ).isEmpty();
  mActionAllEdits->setEnabled( hasEditLayers );
  mActionCancelAllEdits->setEnabled( hasEditLayers );

  bool hasModifiedLayers = !editableLayers( true, true ).isEmpty();
  mActionSaveAllEdits->setEnabled( hasModifiedLayers );
  mActionRollbackAllEdits->setEnabled( hasModifiedLayers );
}

QList<QgsMapLayer *> QgisApp::editableLayers( bool modified, bool ignoreLayersWhichCannotBeToggled ) const
{
  QList<QgsMapLayer *> editLayers;
  // use legend layers (instead of registry) so QList mirrors its order
  const auto constFindLayers = mLayerTreeView->layerTreeModel()->rootGroup()->findLayers();
  for ( QgsLayerTreeLayer *nodeLayer : constFindLayers )
  {
    QgsMapLayer *layer = nodeLayer->layer();
    if ( !layer )
      continue;

    if ( layer->isEditable() && ( !modified || layer->isModified() ) && ( !ignoreLayersWhichCannotBeToggled || !( layer->properties() & Qgis::MapLayerProperty::UsersCannotToggleEditing ) ) )
      editLayers << layer;
  }
  return editLayers;
}

void QgisApp::duplicateVectorStyle( QgsVectorLayer *srcLayer, QgsVectorLayer *destLayer )
{
  // copy symbology, if possible
  if ( srcLayer->geometryType() == destLayer->geometryType() )
  {
    QDomImplementation DomImplementation;
    QDomDocumentType documentType =
      DomImplementation.createDocumentType(
        QStringLiteral( "qgis" ), QStringLiteral( "http://mrcc.com/qgis.dtd" ), QStringLiteral( "SYSTEM" ) );
    QDomDocument doc( documentType );
    QDomElement rootNode = doc.createElement( QStringLiteral( "qgis" ) );
    rootNode.setAttribute( QStringLiteral( "version" ), Qgis::version() );
    doc.appendChild( rootNode );
    QString errorMsg;
    QgsReadWriteContext writeContext = QgsReadWriteContext();
    srcLayer->writeSymbology( rootNode, doc, errorMsg, writeContext );
    QgsReadWriteContext readContext = QgsReadWriteContext();
    destLayer->readSymbology( rootNode, errorMsg, readContext );
  }
}


void QgisApp::layerSubsetString()
{
  layerSubsetString( activeLayer() );
}

void QgisApp::layerSubsetString( QgsMapLayer *mapLayer )
{

  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mapLayer );
  if ( !vlayer )
  {
    // Try PG raster
    QgsRasterLayer *rlayer = qobject_cast<QgsRasterLayer *>( mapLayer );
    if ( rlayer )
    {
      QgsRasterDataProvider *provider = rlayer->dataProvider();
      if ( provider &&
           provider->supportsSubsetString() )
      {
        // PG raster is the only one for now
        if ( provider->name() == QLatin1String( "postgresraster" ) )
        {
          // We need a vector for the sql editor
          QgsDataSourceUri vectorUri { provider->dataSourceUri() };
          vectorUri.setGeometryColumn( QString() );
          vectorUri.setSrid( QString() );
          QgsVectorLayer vlayer { vectorUri.uri( ), QStringLiteral( "pgrasterlayer" ), QStringLiteral( "postgres" ) };
          if ( vlayer.isValid( ) )
          {
            // launch the query builder
            QgsQueryBuilder qb { &vlayer };
            QString subsetBefore = vlayer.subsetString();

            // Set the sql in the query builder to the same in the prop dialog
            // (in case the user has already changed it)
            qb.setSql( rlayer->subsetString() );
            // Open the query builder and refresh symbology if sql has changed
            // Note: repaintRequested is emitted directly from QgsQueryBuilder
            //       when the sql is set in the layer.
            if ( qb.exec() && ( subsetBefore != qb.sql() ) && mLayerTreeView )
            {
              if ( rlayer->setSubsetString( qb.sql() ) )
              {
                mLayerTreeView->refreshLayerSymbology( rlayer->id() );
                activateDeactivateLayerRelatedActions( rlayer );
              }
            }
          }
        }
      }
    }
    QgsPointCloudLayer *pclayer = qobject_cast<QgsPointCloudLayer *>( mapLayer );
    if ( pclayer )
    {
      QgsPointCloudQueryBuilder qb { pclayer };
      qb.setSubsetString( pclayer->subsetString() );
      qb.exec();
    }
    return;
  }


  bool joins = !vlayer->vectorJoins().isEmpty();
  if ( vlayer->vectorJoins().size() == 1 )
  {
    QgsVectorLayerJoinInfo info = vlayer->vectorJoins()[0];
    joins = !vlayer->joinBuffer()->isAuxiliaryJoin( info );
  }

  if ( joins )
  {
    if ( QMessageBox::question( nullptr, tr( "Filter on Joined Fields" ),
                                tr( "You are about to set a subset filter on a layer that has joined fields. "
                                    "Joined fields cannot be filtered, unless you convert the layer to a virtual layer first. "
                                    "Would you like to create a virtual layer out of this layer first?" ),
                                QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
    {
      QgsVirtualLayerDefinition def = QgsVirtualLayerDefinitionUtils::fromJoinedLayer( vlayer );
      const QgsVectorLayer::LayerOptions options { QgsProject::instance()->transformContext() };
      QgsVectorLayer *newLayer = new QgsVectorLayer( def.toString(), vlayer->name() + " (virtual)", QStringLiteral( "virtual" ), options );
      if ( newLayer->isValid() )
      {
        duplicateVectorStyle( vlayer, newLayer );
        QgsProject::instance()->addMapLayer( newLayer, /*addToLegend*/ false, /*takeOwnership*/ true );
        QgsLayerTreeUtils::insertLayerBelow( QgsProject::instance()->layerTreeRoot(), vlayer, newLayer );
        mLayerTreeView->setCurrentLayer( newLayer );
        // hide the old layer
        QgsLayerTreeLayer *vLayerTreeLayer = QgsProject::instance()->layerTreeRoot()->findLayer( vlayer->id() );
        if ( vLayerTreeLayer )
          vLayerTreeLayer->setItemVisibilityChecked( false );
        vlayer = newLayer;
      }
      else
      {
        delete newLayer;
      }
    }
  }

  // launch the query builder
  std::unique_ptr<QgsSubsetStringEditorInterface> qb( QgsGui::subsetStringEditorProviderRegistry()->createDialog( vlayer, this ) );
  QString subsetBefore = vlayer->subsetString();

  // Set the sql in the query builder to the same in the prop dialog
  // (in case the user has already changed it)
  qb->setSubsetString( vlayer->subsetString() );
  // Open the query builder and refresh symbology if sql has changed
  // Note: repaintRequested is emitted directly from QgsQueryBuilder
  //       when the sql is set in the layer.
  if ( qb->exec() && ( subsetBefore != qb->subsetString() ) && mLayerTreeView )
  {
    mLayerTreeView->refreshLayerSymbology( vlayer->id() );
    activateDeactivateLayerRelatedActions( vlayer );
  }
}

void QgisApp::saveLastMousePosition( const QgsPointXY &p )
{
  if ( mMapTipsVisible )
  {
    // store the point, we need it for when the maptips timer fires
    mLastMapPosition = p;

    // we use this slot to control the timer for maptips since it is fired each time
    // the mouse moves.
    if ( mMapCanvas->underMouse() )
    {
      // Clear the maptip (this is done conditionally)
      mpMaptip->clear( mMapCanvas );
      // don't start the timer if the mouse is not over the map canvas
      mpMapTipsTimer->start();
    }
  }
}


void QgisApp::showScale( double scale )
{
  mScaleWidget->setScale( scale );
}


void QgisApp::userRotation()
{
  double degrees = mRotationEdit->value();
  mMapCanvas->setRotation( degrees );
  mMapCanvas->refresh();
}

void QgisApp::projectCrsChanged()
{
  updateCrsStatusBar();
  QgsDebugMsgLevel( QStringLiteral( "QgisApp::setupConnections -1- : QgsProject::instance()->crs().description[%1]ellipsoid[%2]" ).arg( QgsProject::instance()->crs().description(), QgsProject::instance()->crs().ellipsoidAcronym() ), 3 );
  mMapCanvas->setDestinationCrs( QgsProject::instance()->crs() );

  // handle datum transforms
  QList<QgsCoordinateReferenceSystem> alreadyAsked;
  QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( QMap<QString, QgsMapLayer *>::const_iterator it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    if ( !alreadyAsked.contains( it.value()->crs() ) )
    {
      alreadyAsked.append( it.value()->crs() );
      askUserForDatumTransform( it.value()->crs(),
                                QgsProject::instance()->crs(), it.value() );
    }
  }
}

void QgisApp::projectTemporalRangeChanged()
{
  QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  QgsMapLayer *currentLayer = nullptr;

  for ( QMap<QString, QgsMapLayer *>::const_iterator it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    currentLayer = it.value();

    if ( currentLayer->dataProvider() )
    {
      if ( QgsProviderMetadata *metadata = QgsProviderRegistry::instance()->providerMetadata(
                                             currentLayer->providerType() ) )
      {
        QVariantMap uri = metadata->decodeUri( currentLayer->dataProvider()->dataSourceUri() );

        if ( uri.contains( QStringLiteral( "temporalSource" ) ) &&
             uri.value( QStringLiteral( "temporalSource" ) ).toString() == QLatin1String( "project" ) )
        {
          QgsDateTimeRange range = QgsProject::instance()->timeSettings()->temporalRange();
          if ( range.begin().isValid() && range.end().isValid() )
          {
            QString time = range.begin().toString( Qt::ISODateWithMs ) + '/' +
                           range.end().toString( Qt::ISODateWithMs );

            uri[ QStringLiteral( "time" ) ] = time;

            currentLayer->setDataSource( metadata->encodeUri( uri ), currentLayer->name(), currentLayer->providerType(), QgsDataProvider::ProviderOptions() );
          }
        }
      }
    }
  }
}

// toggle overview status
void QgisApp::isInOverview()
{
  mLayerTreeView->defaultActions()->showInOverview();
}

void QgisApp::removingLayers( const QStringList &layers )
{
  const auto constLayers = layers;
  for ( const QString &layerId : constLayers )
  {
    QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>(
                               QgsProject::instance()->mapLayer( layerId ) );
    if ( !vlayer || !vlayer->isEditable() )
      return;

    toggleEditing( vlayer, false );
  }
}

void QgisApp::removeLayer(bool promptConfirmation)
{
  if ( !mLayerTreeView )
  {
    return;
  }

  // look for layers recursively so we catch also those that are within selected groups
  const QList<QgsMapLayer *> selectedLayers = mLayerTreeView->selectedLayersRecursive();

  QStringList nonRemovableLayerNames;
  for ( QgsMapLayer *layer : selectedLayers )
  {
    if ( !layer->flags().testFlag( QgsMapLayer::Removable ) )
      nonRemovableLayerNames << layer->name();
  }
  if ( !nonRemovableLayerNames.isEmpty() )
  {
      if(promptConfirmation == true) // SG add for prompt quiet
          QMessageBox::warning( this, tr( "Required Layers" ),
                                tr( "The following layers are marked as required by the project:\n\n%1\n\nPlease deselect them (or unmark as required) and retry." ).arg( nonRemovableLayerNames.join( QLatin1Char( '\n' ) ) ) );
      return;
  }

  for ( QgsMapLayer *layer : selectedLayers )
  {
    QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
    if ( vlayer && vlayer->isEditable() && !toggleEditing( vlayer, true ) )
      return;
  }

  QStringList activeTaskDescriptions;
  for ( QgsMapLayer *layer : selectedLayers )
  {
    QList< QgsTask * > tasks = QgsApplication::taskManager()->tasksDependentOnLayer( layer );
    if ( !tasks.isEmpty() )
    {
      const auto constTasks = tasks;
      for ( QgsTask *task : constTasks )
      {
        activeTaskDescriptions << tr( " • %1" ).arg( task->description() );
      }
    }
  }

  if ( !activeTaskDescriptions.isEmpty() )
  {
    QMessageBox::warning( this, tr( "Active Tasks" ),
                          tr( "The following tasks are currently running which depend on this layer:\n\n%1\n\nPlease cancel these tasks and retry." ).arg( activeTaskDescriptions.join( QLatin1Char( '\n' ) ) ) );
    return;
  }

  const QList<QgsLayerTreeNode *> selectedNodes = mLayerTreeView->selectedNodes( true );

  //validate selection
  if ( selectedNodes.isEmpty() )
  {
    visibleMessageBar()->pushMessage( tr( "No legend entries selected" ),
                                      tr( "Select the layers and groups you want to remove in the legend." ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  //bool promptConfirmation = QgsSettings().value( QStringLiteral( "qgis/askToDeleteLayers" ), true ).toBool();

  // Don't show prompt to remove a empty group.
  if ( selectedNodes.count() == 1
       && selectedNodes.at( 0 )->nodeType() == QgsLayerTreeNode::NodeGroup
       && selectedNodes.at( 0 )->children().count() == 0 )
  {
    promptConfirmation = false;
  }

  bool shiftHeld = QApplication::queryKeyboardModifiers().testFlag( Qt::ShiftModifier );

  // Check if there are any hidden layer elements and display a confirmation dialog
  QStringList hiddenLayerNames;
  auto harvest = [ &hiddenLayerNames ]( const QgsLayerTreeNode * parent )
  {
    const auto cChildren { parent->children() };
    for ( const auto &c : cChildren )
    {
      if ( QgsLayerTree::isLayer( c ) )
      {
        const auto treeLayer { QgsLayerTree::toLayer( c ) };
        if ( treeLayer->layer() && treeLayer->layer()->flags().testFlag( QgsMapLayer::LayerFlag::Private ) )
        {
          hiddenLayerNames.push_back( treeLayer->layer()->name( ) );
        }
      }
    }
  };

  for ( const QgsLayerTreeNode *n : selectedNodes )
  {
    harvest( n );
  }

  QString message { tr( "Remove %n legend entries?", "number of legend items to remove", selectedNodes.count() ) };
  if ( ! hiddenLayerNames.isEmpty() )
  {
    if ( hiddenLayerNames.count( ) > 10 )
    {
      const int layerCount { hiddenLayerNames.count( ) };
      hiddenLayerNames = hiddenLayerNames.mid( 0, 10 );
      hiddenLayerNames.push_back( tr( "(%n more hidden layer(s))",  "number of hidden layers not shown", layerCount - 10 ) );
    }
    message.append( tr( "The following hidden layers will be removed:\n%1" ).arg( hiddenLayerNames.join( '\n' ) ) );
  }

  if ( !shiftHeld && promptConfirmation && QMessageBox::warning( this, tr( "Remove layers and groups" ), message, QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel )
  {
    return;
  }

  for ( QgsLayerTreeNode *node : selectedNodes )
  {
    if ( QgsLayerTreeGroup *group = qobject_cast< QgsLayerTreeGroup * >( node ) )
    {
      if ( QgsGroupLayer *groupLayer = group->groupLayer() )
      {
        QgsProject::instance()->removeMapLayer( groupLayer );
      }
    }
    QgsLayerTreeGroup *parentGroup = qobject_cast<QgsLayerTreeGroup *>( node->parent() );
    if ( parentGroup )
      parentGroup->removeChildNode( node );
  }

  showStatusMessage( tr( "%n legend entries removed.", "number of removed legend entries", selectedNodes.count() ) );

  refreshMapCanvas();
}

void QgisApp::duplicateLayers( const QList<QgsMapLayer *> &lyrList )
{
  if ( !mLayerTreeView )
  {
    return;
  }

  const QList<QgsMapLayer *> selectedLyrs = lyrList.empty() ? mLayerTreeView->selectedLayers() : lyrList;
  if ( selectedLyrs.empty() )
  {
    return;
  }

  QgsCanvasRefreshBlocker refreshBlocker;
  QgsMapLayer *dupLayer = nullptr;
  QgsMapLayer *newSelection = nullptr;
  QString layerDupName, unSppType;
  QList<QgsMessageBarItem *> msgBars;

  msgBars.reserve( selectedLyrs.size() );
  for ( QgsMapLayer *selectedLyr : selectedLyrs )
  {
    dupLayer = nullptr;
    unSppType.clear();
    layerDupName = selectedLyr->name() + ' ' + tr( "copy" );

    switch ( selectedLyr->type() )
    {
      case QgsMapLayerType::PluginLayer:
        unSppType = tr( "Plugin layer" );
        break;

      case QgsMapLayerType::GroupLayer:
        unSppType = tr( "Group layer" );
        break;

      case QgsMapLayerType::VectorLayer:
      {
        if ( QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( selectedLyr ) )
        {
          if ( vlayer->auxiliaryLayer() )
            vlayer->auxiliaryLayer()->save();

          dupLayer = vlayer->clone();
        }
        break;
      }

      case QgsMapLayerType::PointCloudLayer:
      case QgsMapLayerType::RasterLayer:
      case QgsMapLayerType::VectorTileLayer:
      case QgsMapLayerType::MeshLayer:
      case QgsMapLayerType::AnnotationLayer:
      {
        dupLayer = selectedLyr->clone();
        break;
      }

    }

    if ( dupLayer && !dupLayer->isValid() )
    {
      msgBars.append( new QgsMessageBarItem(
                        tr( "Duplicate layer: " ),
                        tr( "%1 (duplication resulted in invalid layer)" ).arg( selectedLyr->name() ),
                        Qgis::MessageLevel::Warning,
                        0,
                        mInfoBar ) );
      continue;
    }
    else if ( !unSppType.isEmpty() || !dupLayer )
    {
      msgBars.append( new QgsMessageBarItem(
                        tr( "Duplicate layer: " ),
                        tr( "%1 (%2 type unsupported)" )
                        .arg( selectedLyr->name(),
                              !unSppType.isEmpty() ? QStringLiteral( "'" ) + unSppType + "' " : QString() ),
                        Qgis::MessageLevel::Warning,
                        0,
                        mInfoBar ) );
      continue;
    }

    dupLayer->setName( layerDupName );

    // add layer to layer registry and legend
    QList<QgsMapLayer *> myList;
    myList << dupLayer;
    QgsProject::instance()->layerTreeRegistryBridge()->setEnabled( false );
    QgsProject::instance()->addMapLayers( myList );
    QgsProject::instance()->layerTreeRegistryBridge()->setEnabled( true );

    QgsLayerTreeLayer *nodeSelectedLyr = mLayerTreeView->layerTreeModel()->rootGroup()->findLayer( selectedLyr->id() );
    Q_ASSERT( nodeSelectedLyr );
    Q_ASSERT( QgsLayerTree::isGroup( nodeSelectedLyr->parent() ) );
    QgsLayerTreeGroup *parentGroup = QgsLayerTree::toGroup( nodeSelectedLyr->parent() );

    QgsLayerTreeLayer *nodeDupLayer = parentGroup->insertLayer( parentGroup->children().indexOf( nodeSelectedLyr ) + 1, dupLayer );

    // always set duplicated layers to not visible so layer can be configured before being turned on
    nodeDupLayer->setItemVisibilityChecked( false );

    // duplicate the layer style
    QString errMsg;
    QDomDocument style;
    QgsReadWriteContext context;
    selectedLyr->exportNamedStyle( style, errMsg, context );
    if ( errMsg.isEmpty() )
      dupLayer->importNamedStyle( style, errMsg );
    if ( !errMsg.isEmpty() )
      visibleMessageBar()->pushMessage( errMsg,
                                        tr( "Cannot copy style to duplicated layer." ),
                                        Qgis::MessageLevel::Critical );
    else if ( qobject_cast<QgsVectorLayer *>( dupLayer ) )
      visibleMessageBar()->pushMessage( tr( "Layer duplication complete" ),
                                        dupLayer->providerType() != QLatin1String( "memory" ) ? tr( "Note that it's using the same data source." ) : QString(),
                                        Qgis::MessageLevel::Info );

    if ( !newSelection )
      newSelection = dupLayer;
  }

  dupLayer = nullptr;

  // auto select first new duplicate layer
  if ( newSelection )
    setActiveLayer( newSelection );

  // display errors in message bar after duplication of layers
  for ( QgsMessageBarItem *msgBar : std::as_const( msgBars ) )
  {
    mInfoBar->pushItem( msgBar );
  }
}

void QgisApp::setLayerScaleVisibility()
{
  if ( !mLayerTreeView )
    return;

  QList<QgsMapLayer *> layers = mLayerTreeView->selectedLayers();

  if ( layers.length() < 1 )
    return;

  QgsScaleVisibilityDialog *dlg = new QgsScaleVisibilityDialog( this, tr( "Set scale visibility for selected layers" ), mMapCanvas );
  QgsMapLayer *layer = mLayerTreeView->currentLayer();
  if ( layer )
  {
    dlg->setScaleVisiblity( layer->hasScaleBasedVisibility() );
    dlg->setMinimumScale( layer->minimumScale() );
    dlg->setMaximumScale( layer->maximumScale() );
  }
  if ( dlg->exec() )
  {
    QgsCanvasRefreshBlocker refreshBlocker;
    const auto constLayers = layers;
    for ( QgsMapLayer *layer : constLayers )
    {
      layer->setScaleBasedVisibility( dlg->hasScaleVisibility() );
      layer->setMaximumScale( dlg->maximumScale() );
      layer->setMinimumScale( dlg->minimumScale() );
    }
  }
  delete dlg;
}

void QgisApp::zoomToLayerScale()
{
  if ( !mLayerTreeView )
    return;

  QList<QgsMapLayer *> layers = mLayerTreeView->selectedLayers();

  if ( layers.length() < 1 )
    return;

  QgsMapLayer *layer = mLayerTreeView->currentLayer();
  if ( layer && layer->hasScaleBasedVisibility() )
  {
    const double scale = mMapCanvas->scale();
    if ( scale > layer->minimumScale() && layer->minimumScale() > 0 )
    {
      mMapCanvas->zoomScale( layer->minimumScale() * Qgis::SCALE_PRECISION );
    }
    else if ( scale <= layer->maximumScale() && layer->maximumScale() > 0 )
    {
      mMapCanvas->zoomScale( layer->maximumScale() );
    }
  }
}

void QgisApp::setLayerCrs()
{
  if ( !( mLayerTreeView && mLayerTreeView->currentLayer() ) )
  {
    return;
  }

  QgsProjectionSelectionDialog mySelector( this );
  mySelector.setCrs( mLayerTreeView->currentLayer()->crs() );

  if ( !mLayerTreeView->currentLayer()->crs().isValid() )
    mySelector.showNoCrsForLayerMessage();

  if ( !mySelector.exec() )
  {
    QApplication::restoreOverrideCursor();
    return;
  }

  QgsCoordinateReferenceSystem crs = mySelector.crs();

  const auto constSelectedNodes = mLayerTreeView->selectedNodes();
  for ( QgsLayerTreeNode *node : constSelectedNodes )
  {
    if ( QgsLayerTree::isGroup( node ) )
    {
      const auto constFindLayers = QgsLayerTree::toGroup( node )->findLayers();
      for ( QgsLayerTreeLayer *child : constFindLayers )
      {
        if ( child->layer() )
        {
          askUserForDatumTransform( crs, QgsProject::instance()->crs(), child->layer() );
          child->layer()->setCrs( crs );
          child->layer()->triggerRepaint();
        }
      }
    }
    else if ( QgsLayerTree::isLayer( node ) )
    {
      QgsLayerTreeLayer *nodeLayer = QgsLayerTree::toLayer( node );
      if ( nodeLayer->layer() )
      {
        askUserForDatumTransform( crs, QgsProject::instance()->crs(), nodeLayer->layer() );
        nodeLayer->layer()->setCrs( crs );
        nodeLayer->layer()->triggerRepaint();
      }
    }
  }

  refreshMapCanvas();
}

void QgisApp::setProjectCrsFromLayer()
{
  if ( !( mLayerTreeView && mLayerTreeView->currentLayer() ) )
  {
    return;
  }

  QgsCoordinateReferenceSystem crs = mLayerTreeView->currentLayer()->crs();
  QgsCanvasRefreshBlocker refreshBlocker;
  QgsProject::instance()->setCrs( crs );
}


void QgisApp::legendLayerZoomNative()
{
  if ( !mLayerTreeView )
    return;

  //find current Layer
  QgsMapLayer *currentLayer = mLayerTreeView->currentLayer();
  if ( !currentLayer )
    return;

  if ( QgsRasterLayer *layer = qobject_cast<QgsRasterLayer *>( currentLayer ) )
  {
    QgsDebugMsgLevel( "Raster units per pixel  : " + QString::number( layer->rasterUnitsPerPixelX() ), 2 );
    QgsDebugMsgLevel( "MapUnitsPerPixel before : " + QString::number( mMapCanvas->mapUnitsPerPixel() ), 2 );

    QList< double >nativeResolutions;
    if ( layer->dataProvider() )
    {
      nativeResolutions = layer->dataProvider()->nativeResolutions();
    }

    // get length of central canvas pixel width in source raster crs
    QgsRectangle e = mMapCanvas->extent();
    QSize s = mMapCanvas->mapSettings().outputSize();
    QgsPointXY p1( e.center().x(), e.center().y() );
    QgsPointXY p2( e.center().x() + e.width() / s.width(), e.center().y() + e.height() / s.height() );
    QgsCoordinateTransform ct( mMapCanvas->mapSettings().destinationCrs(), layer->crs(), QgsProject::instance() );
    p1 = ct.transform( p1 );
    p2 = ct.transform( p2 );
    const double diagonalSize = std::sqrt( p1.sqrDist( p2 ) ); // width (actually the diagonal) of reprojected pixel
    if ( !nativeResolutions.empty() )
    {
      // find closest native resolution
      QList< double > diagonalNativeResolutions;
      diagonalNativeResolutions.reserve( nativeResolutions.size() );
      for ( double d : nativeResolutions )
        diagonalNativeResolutions << std::sqrt( 2 * d * d );

      int i;
      for ( i = 0; i < diagonalNativeResolutions.size() && diagonalNativeResolutions.at( i ) < diagonalSize; i++ )
      {
        QgsDebugMsgLevel( QStringLiteral( "test resolution %1: %2" ).arg( i ).arg( diagonalNativeResolutions.at( i ) ), 2 );
      }
      if ( i == nativeResolutions.size() ||
           ( i > 0 && ( ( diagonalNativeResolutions.at( i ) - diagonalSize ) > ( diagonalSize - diagonalNativeResolutions.at( i - 1 ) ) ) ) )
      {
        QgsDebugMsgLevel( QStringLiteral( "previous resolution" ), 2 );
        i--;
      }

      mMapCanvas->zoomByFactor( nativeResolutions.at( i ) / mMapCanvas->mapUnitsPerPixel() );
    }
    else
    {
      mMapCanvas->zoomByFactor( std::sqrt( layer->rasterUnitsPerPixelX() * layer->rasterUnitsPerPixelX() + layer->rasterUnitsPerPixelY() * layer->rasterUnitsPerPixelY() ) / diagonalSize );
    }

    mMapCanvas->refresh();
    QgsDebugMsgLevel( "MapUnitsPerPixel after  : " + QString::number( mMapCanvas->mapUnitsPerPixel() ), 2 );
  }
}

void QgisApp::legendLayerStretchUsingCurrentExtent()
{
  if ( !mLayerTreeView )
    return;

  //find current Layer
  QgsMapLayer *currentLayer = mLayerTreeView->currentLayer();
  if ( !currentLayer )
    return;

  QgsRasterLayer *layer = qobject_cast<QgsRasterLayer *>( currentLayer );
  if ( layer )
  {
    QgsRectangle myRectangle;
    myRectangle = mMapCanvas->mapSettings().outputExtentToLayerExtent( layer, mMapCanvas->extent() );
    layer->refreshContrastEnhancement( myRectangle );

    mLayerTreeView->refreshLayerSymbology( layer->id() );
    refreshMapCanvas();
  }
}

void QgisApp::applyStyleToGroup()
{
  if ( !mLayerTreeView )
    return;

  const auto constSelectedNodes = mLayerTreeView->selectedNodes();
  for ( QgsLayerTreeNode *node : constSelectedNodes )
  {
    if ( QgsLayerTree::isGroup( node ) )
    {
      const auto constFindLayers = QgsLayerTree::toGroup( node )->findLayers();
      for ( QgsLayerTreeLayer *nodeLayer : constFindLayers )
      {
        if ( nodeLayer->layer() )
        {
          pasteStyle( nodeLayer->layer() );
        }
      }
    }
    else if ( QgsLayerTree::isLayer( node ) )
    {
      QgsLayerTreeLayer *nodeLayer = QgsLayerTree::toLayer( node );
      if ( nodeLayer->layer() )
      {
        pasteStyle( nodeLayer->layer() );
      }
    }
  }
}

void QgisApp::legendGroupSetCrs()
{
  if ( !mMapCanvas )
  {
    return;
  }

  QgsLayerTreeGroup *currentGroup = mLayerTreeView->currentGroupNode();
  if ( !currentGroup )
    return;

  QgsProjectionSelectionDialog mySelector( this );
  if ( !mySelector.exec() )
  {
    QApplication::restoreOverrideCursor();
    return;
  }

  QgsCoordinateReferenceSystem crs = mySelector.crs();
  const auto constFindLayers = currentGroup->findLayers();
  for ( QgsLayerTreeLayer *nodeLayer : constFindLayers )
  {
    if ( nodeLayer->layer() )
    {
      nodeLayer->layer()->setCrs( crs );
      nodeLayer->layer()->triggerRepaint();
    }
  }
}

void QgisApp::legendGroupSetWmsData()
{
  QgsLayerTreeGroup *currentGroup = mLayerTreeView->currentGroupNode();
  if ( !currentGroup )
    return;
  QgsGroupWmsDataDialog *dlg = new QgsGroupWmsDataDialog( this );
  dlg->setGroupShortName( currentGroup->customProperty( QStringLiteral( "wmsShortName" ) ).toString() );
  dlg->setGroupTitle( currentGroup->customProperty( QStringLiteral( "wmsTitle" ) ).toString() );
  dlg->setGroupAbstract( currentGroup->customProperty( QStringLiteral( "wmsAbstract" ) ).toString() );
  if ( dlg->exec() )
  {
    currentGroup->setCustomProperty( QStringLiteral( "wmsShortName" ), dlg->groupShortName() );
    currentGroup->setCustomProperty( QStringLiteral( "wmsTitle" ), dlg->groupTitle() );
    currentGroup->setCustomProperty( QStringLiteral( "wmsAbstract" ), dlg->groupAbstract() );
  }
  delete dlg;
}

void QgisApp::zoomToLayerExtent()
{
  mLayerTreeView->defaultActions()->zoomToLayers( mMapCanvas );
}

void QgisApp::showPluginManager()
{
#ifdef WITH_BINDINGS
  if ( mPythonUtils && mPythonUtils->isEnabled() )
  {
    // Call pluginManagerInterface()->showPluginManager() as soon as the plugin installer says the remote data is fetched.
    QgsPythonRunner::run( QStringLiteral( "pyplugin_installer.instance().showPluginManagerWhenReady()" ) );
  }
  else
#endif
  {
    // Call the pluginManagerInterface directly
    mQgisInterface->pluginManagerInterface()->showPluginManager();
  }
}


// implementation of the python runner
class QgsPythonRunnerImpl : public QgsPythonRunner
{
  public:
    explicit QgsPythonRunnerImpl( QgsPythonUtils *pythonUtils ) : mPythonUtils( pythonUtils ) {}

    bool runCommand( QString command, QString messageOnError = QString() ) override
    {
#ifdef WITH_BINDINGS
      if ( mPythonUtils && mPythonUtils->isEnabled() )
      {
        return mPythonUtils->runString( command, messageOnError, false );
      }
#else
      Q_UNUSED( command )
      Q_UNUSED( messageOnError )
#endif
      return false;
    }

    bool evalCommand( QString command, QString &result ) override
    {
#ifdef WITH_BINDINGS
      if ( mPythonUtils && mPythonUtils->isEnabled() )
      {
        return mPythonUtils->evalString( command, result );
      }
#else
      Q_UNUSED( command )
      Q_UNUSED( result )
#endif
      return false;
    }

  protected:
    QgsPythonUtils *mPythonUtils = nullptr;
};

void QgisApp::loadPythonSupport()
{
  QgsScopedRuntimeProfile profile( tr( "Loading Python support" ) );

  QString pythonlibName( QStringLiteral( "qgispython" ) );
#if defined(Q_OS_UNIX)
  pythonlibName.prepend( QgsApplication::libraryPath() );
#endif
#ifdef __MINGW32__
  pythonlibName.prepend( "lib" );
#endif
  QString version = QStringLiteral( "%1.%2.%3" ).arg( Qgis::versionInt() / 10000 ).arg( Qgis::versionInt() / 100 % 100 ).arg( Qgis::versionInt() % 100 );
  QgsDebugMsgLevel( QStringLiteral( "load library %1 (%2)" ).arg( pythonlibName, version ), 2 );
  QLibrary pythonlib( pythonlibName, version );
  // It's necessary to set these two load hints, otherwise Python library won't work correctly
  // see http://lists.kde.org/?l=pykde&m=117190116820758&w=2
  pythonlib.setLoadHints( QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint );
  if ( !pythonlib.load() )
  {
    pythonlib.setFileName( pythonlibName );
    if ( !pythonlib.load() )
    {
      QgsMessageLog::logMessage( tr( "Couldn't load Python support library: %1" ).arg( pythonlib.errorString() ) );
      return;
    }
  }

#ifdef WITH_BINDINGS
  typedef QgsPythonUtils*( *inst )();
  inst pythonlib_inst = reinterpret_cast< inst >( cast_to_fptr( pythonlib.resolve( "instance" ) ) );
  if ( !pythonlib_inst )
  {
    //using stderr on purpose because we want end users to see this [TS]
    QgsMessageLog::logMessage( tr( "Couldn't resolve python support library's instance() symbol." ) );
    return;
  }

  mPythonUtils = pythonlib_inst();
  if ( mPythonUtils )
  {
    QgsCrashHandler::sPythonCrashLogFile = QStandardPaths::standardLocations( QStandardPaths::TempLocation ).at( 0 ) + "/qgis-python-crash-info-" + QString::number( QCoreApplication::applicationPid() );
    mPythonUtils->initPython( mQgisInterface, true, QgsCrashHandler::sPythonCrashLogFile );
  }

  if ( mPythonUtils && mPythonUtils->isEnabled() )
  {
    QgsPluginRegistry::instance()->setPythonUtils( mPythonUtils );

    // init python runner
    QgsPythonRunner::setInstance( new QgsPythonRunnerImpl( mPythonUtils ) );

    // QgsMessageLog::logMessage( tr( "Python support ENABLED :-) " ), QString(), Qgis::MessageLevel::Info );
  }
#endif
}

void QgisApp::checkQgisVersion()
{
  QgsVersionInfo *versionInfo = new QgsVersionInfo();
  QApplication::setOverrideCursor( Qt::WaitCursor );

  connect( versionInfo, &QgsVersionInfo::versionInfoAvailable, this, &QgisApp::versionReplyFinished );
  versionInfo->checkVersion();
}

void QgisApp::versionReplyFinished()
{
  QApplication::restoreOverrideCursor();

  QgsVersionInfo *versionInfo = qobject_cast<QgsVersionInfo *>( sender() );
  Q_ASSERT( versionInfo );

  if ( versionInfo->error() == QNetworkReply::NoError )
  {
    QString info;

    if ( versionInfo->newVersionAvailable() )
    {
      info = tr( "There is a new version of QGIS available" );
    }
    else if ( versionInfo->isDevelopmentVersion() )
    {
      info = tr( "You are running a development version of QGIS" );
    }
    else
    {
      info = tr( "You are running the current version of QGIS" );
    }

    info = QStringLiteral( "<b>%1</b>" ).arg( info );

    if ( versionInfo->newVersionAvailable() )
      info += "<br>" + QgsStringUtils::insertLinks( versionInfo->downloadInfo() );

    QMessageBox mb( QMessageBox::Information, tr( "QGIS Version Information" ), info );
    mb.setInformativeText( versionInfo->html() );
    mb.exec();
  }
  else
  {
    QMessageBox mb( QMessageBox::Warning, tr( "QGIS Version Information" ), tr( "Unable to get current version information from server" ) );
    mb.setDetailedText( versionInfo->errorString() );
    mb.exec();
  }
}

void QgisApp::configureShortcuts()
{
  QgsConfigureShortcutsDialog dlg( this );
  dlg.exec();
}

void QgisApp::customize()
{
  QgsCustomization::instance()->openDialog( this );
}

void QgisApp::options()
{
  showOptionsDialog( this );
}

QMap< QString, QString > QgisApp::projectPropertiesPagesMap()
{
  static QMap< QString, QString > sProjectPropertiesPagesMap;
  static std::once_flag initialized;
  std::call_once( initialized, []
  {
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "General" ), QStringLiteral( "mProjOptsGeneral" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "Metadata" ), QStringLiteral( "mMetadataPage" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "View Settings" ), QStringLiteral( "mViewSettingsPage" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "CRS" ), QStringLiteral( "mProjOptsCRS" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "Transformations" ), QStringLiteral( "mProjTransformations" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "Default Styles" ), QStringLiteral( "mProjOptsSymbols" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "Data Sources" ), QStringLiteral( "mTab_DataSources" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "Relations" ), QStringLiteral( "mTabRelations" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "Variables" ), QStringLiteral( "mTab_Variables" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "Macros" ), QStringLiteral( "mProjOptsMacros" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "QGIS Server" ), QStringLiteral( "mProjOptsOWS" ) );
    sProjectPropertiesPagesMap.insert( QCoreApplication::translate( "QgsProjectPropertiesBase", "Temporal" ), QStringLiteral( "mTemporalOptions" ) );
  } );

  for ( const QPointer< QgsOptionsWidgetFactory > &f : std::as_const( mProjectPropertiesWidgetFactories ) )
  {
    // remove any deleted factories
    if ( f )
    {
      sProjectPropertiesPagesMap.insert( f->title(), f->title() );
    }
  }

  return sProjectPropertiesPagesMap;
}

void QgisApp::showProjectProperties( const QString &page )
{
  projectProperties( page );
}

QMap< QString, QString > QgisApp::settingPagesMap()
{
  static QMap< QString, QString > sSettingPagesMap;
  static std::once_flag initialized;
  std::call_once( initialized, []
  {
    sSettingPagesMap.insert( tr( "Style Manager" ), QStringLiteral( "stylemanager" ) );
    sSettingPagesMap.insert( tr( "Keyboard Shortcuts" ), QStringLiteral( "shortcuts" ) );
    sSettingPagesMap.insert( tr( "Custom Projections" ), QStringLiteral( "customprojection" ) );
    sSettingPagesMap.insert( tr( "Interface Customization" ), QStringLiteral( "customize" ) );
  } );

  return sSettingPagesMap;
}

void QgisApp::showSettings( const QString &page )
{
  if ( page == QLatin1String( "stylemanager" ) )
  {
    showStyleManager();
  }
  else if ( page == QLatin1String( "shortcuts" ) )
  {
    configureShortcuts();
  }
  else if ( page == QLatin1String( "customprojection" ) )
  {
    customProjection();
  }
  else if ( page == QLatin1String( "customize" ) )
  {
    customize();
  }
}

QMap<QString, QString> QgisApp::optionsPagesMap()
{
  static QMap< QString, QString > sOptionsPagesMap;
  static std::once_flag initialized;
  std::call_once( initialized, []
  {
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "General" ), QStringLiteral( "mOptionsPageGeneral" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "System" ), QStringLiteral( "mOptionsPageSystem" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "CRS Handling" ), QStringLiteral( "mOptionsPageCRS" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Coordinate Transforms" ), QStringLiteral( "mOptionsPageTransformations" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Data Sources" ), QStringLiteral( "mOptionsPageDataSources" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "GDAL" ), QStringLiteral( "mOptionsPageGDAL" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Rendering" ), QStringLiteral( "mOptionsPageRendering" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Canvas & Legend" ), QStringLiteral( "mOptionsPageMapCanvas" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Map Tools" ), QStringLiteral( "mOptionsPageMapTools" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Colors" ), QStringLiteral( "mOptionsPageColors" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Digitizing" ), QStringLiteral( "mOptionsPageDigitizing" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Layouts" ), QStringLiteral( "mOptionsPageComposer" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Variables" ), QStringLiteral( "mOptionsPageVariables" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Authentication" ), QStringLiteral( "mOptionsPageAuth" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Network" ), QStringLiteral( "mOptionsPageNetwork" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Locator" ), QStringLiteral( "mOptionsLocatorSettings" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Acceleration" ), QStringLiteral( "mOptionsPageAcceleration" ) );
    sOptionsPagesMap.insert( QCoreApplication::translate( "QgsOptionsBase", "Advanced" ), QCoreApplication::translate( "QgsOptionsBase", "Advanced" ) );
  } );

  QMap< QString, QString > pages = sOptionsPagesMap;
  for ( const QPointer< QgsOptionsWidgetFactory > &f : std::as_const( mOptionsWidgetFactories ) )
  {
    // remove any deleted factories
    if ( f )
    {
      pages.insert( f->title(), f->title() );
    }
  }
  return pages;
}

QgsOptions *QgisApp::createOptionsDialog( QWidget *parent )
{
  QList< QgsOptionsWidgetFactory * > factories;
  const auto constMOptionsWidgetFactories = mOptionsWidgetFactories;
  for ( const QPointer< QgsOptionsWidgetFactory > &f : constMOptionsWidgetFactories )
  {
    // remove any deleted factories
    if ( f )
      factories << f;
  }
  return new QgsOptions( parent, QgsGuiUtils::ModalDialogFlags, factories );
}


void QgisApp::showOptionsDialog( QWidget *parent, const QString &currentPage, int pageNumber )
{
  std::unique_ptr< QgsOptions > optionsDialog( createOptionsDialog( parent ) );

  QgsSettings mySettings;
  QString oldScales = mySettings.value( QStringLiteral( "Map/scales" ), Qgis::defaultProjectScales() ).toString();

  if ( !currentPage.isEmpty() )
  {
    optionsDialog->setCurrentPage( currentPage );
  }

  if ( pageNumber >= 0 )
  {
    optionsDialog->setCurrentPage( pageNumber );
  }

  if ( optionsDialog->exec() )
  {
    QgsProject::instance()->layerTreeRegistryBridge()->setNewLayersVisible( mySettings.value( QStringLiteral( "qgis/new_layers_visible" ), true ).toBool() );

    setupLayerTreeViewFromSettings();

    const auto canvases = mapCanvases();
    for ( QgsMapCanvas *canvas : canvases )
    {
      applyDefaultSettingsToCanvas( canvas );
    }

    //update any open compositions so they reflect new composer settings
    //we have to push the changes to the compositions here, because compositions
    //have no access to qgisapp and accordingly can't listen in to changes
    const QList< QgsMasterLayoutInterface * > layouts = QgsProject::instance()->layoutManager()->layouts() ;
    for ( QgsMasterLayoutInterface *layout : layouts )
    {
      layout->updateSettings();
    }

    //do we need this? TS
    for ( QgsMapCanvas *canvas : canvases )
    {
      canvas->refresh();
    }

    mRasterFileFilter = QgsProviderRegistry::instance()->fileRasterFilters();

    if ( oldScales != mySettings.value( QStringLiteral( "Map/scales" ), Qgis::defaultProjectScales() ).toString() )
    {
      mScaleWidget->updateScales();
    }

    mMapTools->mapTool< QgsMeasureTool >( QgsAppMapTools::MeasureDistance )->updateSettings();
    mMapTools->mapTool< QgsMeasureTool >( QgsAppMapTools::MeasureArea )->updateSettings();
    mMapTools->mapTool< QgsMapToolMeasureAngle >( QgsAppMapTools::MeasureAngle )->updateSettings();
    mMapTools->mapTool< QgsMapToolMeasureBearing >( QgsAppMapTools::MeasureBearing )->updateSettings();

#ifdef HAVE_3D
    for ( Qgs3DMapCanvasWidget *canvas3D : mOpen3DMapViews )
    {
      canvas3D->measurementLineTool()->updateSettings();
    }
#endif

    double factor = mySettings.value( QStringLiteral( "qgis/magnifier_factor_default" ), 1.0 ).toDouble();
    mMagnifierWidget->setDefaultFactor( factor );
    mMagnifierWidget->updateMagnification( factor );

    mWelcomePage->updateNewsFeedVisibility();
  }
}

void QgisApp::fullHistogramStretch()
{
  histogramStretch( false, QgsRasterMinMaxOrigin::MinMax );
}

void QgisApp::localHistogramStretch()
{
  histogramStretch( true, QgsRasterMinMaxOrigin::MinMax );
}

void QgisApp::fullCumulativeCutStretch()
{
  histogramStretch( false, QgsRasterMinMaxOrigin::CumulativeCut );
}

void QgisApp::localCumulativeCutStretch()
{
  histogramStretch( true, QgsRasterMinMaxOrigin::CumulativeCut );
}

void QgisApp::histogramStretch( bool visibleAreaOnly, QgsRasterMinMaxOrigin::Limits limits )
{
  QgsMapLayer *myLayer = mLayerTreeView->currentLayer();

  if ( !myLayer )
  {
    visibleMessageBar()->pushMessage( tr( "No Layer Selected" ),
                                      tr( "To perform a full histogram stretch, you need to have a raster layer selected." ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  QgsRasterLayer *myRasterLayer = qobject_cast<QgsRasterLayer *>( myLayer );
  if ( !myRasterLayer )
  {
    visibleMessageBar()->pushMessage( tr( "No Layer Selected" ),
                                      tr( "To perform a full histogram stretch, you need to have a raster layer selected." ),
                                      Qgis::MessageLevel::Info );
    return;
  }

  QgsRectangle myRectangle;
  if ( visibleAreaOnly )
    myRectangle = mMapCanvas->mapSettings().outputExtentToLayerExtent( myRasterLayer, mMapCanvas->extent() );

  myRasterLayer->setContrastEnhancement( QgsContrastEnhancement::StretchToMinimumMaximum, limits, myRectangle );

  myRasterLayer->triggerRepaint();
}

void QgisApp::increaseBrightness()
{
  int step = 1;
  if ( QgsApplication::keyboardModifiers() == Qt::ShiftModifier )
  {
    step = 10;
  }
  adjustBrightnessContrast( step );
}

void QgisApp::decreaseBrightness()
{
  int step = -1;
  if ( QgsApplication::keyboardModifiers() == Qt::ShiftModifier )
  {
    step = -10;
  }
  adjustBrightnessContrast( step );
}

void QgisApp::increaseContrast()
{
  int step = 1;
  if ( QgsApplication::keyboardModifiers() == Qt::ShiftModifier )
  {
    step = 10;
  }
  adjustBrightnessContrast( step, false );
}

void QgisApp::decreaseContrast()
{
  int step = -1;
  if ( QgsApplication::keyboardModifiers() == Qt::ShiftModifier )
  {
    step = -10;
  }
  adjustBrightnessContrast( step, false );
}

void QgisApp::adjustBrightnessContrast( int delta, bool updateBrightness )
{
  const auto constSelectedLayers = mLayerTreeView->selectedLayers();
  for ( QgsMapLayer *layer : constSelectedLayers )
  {
    if ( !layer )
    {
      visibleMessageBar()->pushMessage( tr( "No Layer Selected" ),
                                        tr( "To change brightness or contrast, you need to have a raster layer selected." ),
                                        Qgis::MessageLevel::Info );
      return;
    }

    QgsRasterLayer *rasterLayer = qobject_cast<QgsRasterLayer *>( layer );
    if ( !rasterLayer )
    {
      visibleMessageBar()->pushMessage( tr( "No Layer Selected" ),
                                        tr( "To change brightness or contrast, you need to have a raster layer selected." ),
                                        Qgis::MessageLevel::Info );
      return;
    }

    QgsBrightnessContrastFilter *brightnessFilter = rasterLayer->brightnessFilter();

    if ( updateBrightness )
    {
      brightnessFilter->setBrightness( brightnessFilter->brightness() + delta );
    }
    else
    {
      brightnessFilter->setContrast( brightnessFilter->contrast() + delta );
    }

    rasterLayer->triggerRepaint();
  }
}

void QgisApp::increaseGamma()
{
  double step = 0.1;
  if ( QgsApplication::keyboardModifiers() == Qt::ShiftModifier )
  {
    step = 1.0;
  }
  adjustGamma( step );
}

void QgisApp::decreaseGamma()
{
  double step = -0.1;
  if ( QgsApplication::keyboardModifiers() == Qt::ShiftModifier )
  {
    step = -1.0;
  }
  adjustGamma( step );
}

void QgisApp::adjustGamma( double delta )
{
  const auto constSelectedLayers = mLayerTreeView->selectedLayers();
  for ( QgsMapLayer *layer : constSelectedLayers )
  {
    if ( !layer )
    {
      visibleMessageBar()->pushMessage( tr( "No Layer Selected" ),
                                        tr( "To change gamma, you need to have a raster layer selected." ),
                                        Qgis::MessageLevel::Info );
      return;
    }

    QgsRasterLayer *rasterLayer = qobject_cast<QgsRasterLayer *>( layer );
    if ( !rasterLayer )
    {
      visibleMessageBar()->pushMessage( tr( "No Layer Selected" ),
                                        tr( "To change gamma, you need to have a raster layer selected." ),
                                        Qgis::MessageLevel::Info );
      return;
    }

    QgsBrightnessContrastFilter *brightnessFilter = rasterLayer->brightnessFilter();
    brightnessFilter->setGamma( brightnessFilter->gamma() + delta );

    rasterLayer->triggerRepaint();
  }
}

void QgisApp::helpContents()
{
  QgsHelp::openHelp( QStringLiteral( "index.html" ) );
}

void QgisApp::apiDocumentation()
{
  if ( QFileInfo::exists( QgsApplication::pkgDataPath() + "/doc/api/index.html" ) )
  {
    openURL( QStringLiteral( "api/index.html" ) );
  }
  else
  {
    QgsSettings settings;
    QString QgisApiUrl = settings.value( QStringLiteral( "qgis/QgisApiUrl" ),
                                         QStringLiteral( "https://qgis.org/api/" ) ).toString();
    openURL( QgisApiUrl, false );
  }
}

void QgisApp::reportaBug()
{
  QgsSettings settings;
  QString reportaBugUrl = settings.value( QStringLiteral( "qgis/reportaBugUrl" ),
                                          tr( "https://qgis.org/en/site/getinvolved/development/bugreporting.html" ) ).toString();
  openURL( reportaBugUrl, false );
}

void QgisApp::supportProviders()
{
  QgsSettings settings;
  QString supportProvidersUrl = settings.value( QStringLiteral( "qgis/supportProvidersUrl" ),
                                tr( "https://qgis.org/en/site/forusers/commercial_support.html" ) ).toString();
  openURL( supportProvidersUrl, false );
}

void QgisApp::helpQgisHomePage()
{
  QgsSettings settings;
  QString  helpQgisHomePageUrl = settings.value( QStringLiteral( "qgis/helpQgisHomePageUrl" ),
                                 QStringLiteral( "https://qgis.org" ) ).toString();
  openURL( helpQgisHomePageUrl, false );
}

void QgisApp::openURL( QString url, bool useQgisDocDirectory )
{
  // open help in user browser
  if ( useQgisDocDirectory )
  {
    url = "file://" + QgsApplication::pkgDataPath() + "/doc/" + url;
  }
#ifdef Q_OS_MACX
  /* Use Mac OS X Launch Services which uses the user's default browser
   * and will just open a new window if that browser is already running.
   * QProcess creates a new browser process for each invocation and expects a
   * commandline application rather than a bundled application.
   */
  CFURLRef urlRef = CFURLCreateWithBytes( kCFAllocatorDefault,
                                          reinterpret_cast<const UInt8 *>( url.toUtf8().constData() ), url.length(),
                                          kCFStringEncodingUTF8, nullptr );
  OSStatus status = LSOpenCFURLRef( urlRef, nullptr );
  status = 0; //avoid compiler warning
  CFRelease( urlRef );
#elif defined(Q_OS_WIN)
  if ( url.startsWith( "file://", Qt::CaseInsensitive ) )
    ShellExecuteA( 0, 0, url.mid( 7 ).toLocal8Bit().constData(), 0, 0, SW_SHOWNORMAL ); // SG change ShellExecute to ShellExecuteA
  else
    QDesktopServices::openUrl( url );
#else
  QDesktopServices::openUrl( url );
#endif
}

void QgisApp::registerMapLayerPropertiesFactory( QgsMapLayerConfigWidgetFactory *factory )
{
  mMapLayerPanelFactories << factory;
  if ( mMapStyleWidget )
    mMapStyleWidget->setPageFactories( mMapLayerPanelFactories );
}

void QgisApp::unregisterMapLayerPropertiesFactory( QgsMapLayerConfigWidgetFactory *factory )
{
  mMapLayerPanelFactories.removeAll( factory );
  if ( mMapStyleWidget )
    mMapStyleWidget->setPageFactories( mMapLayerPanelFactories );
}

void QgisApp::registerOptionsWidgetFactory( QgsOptionsWidgetFactory *factory )
{
  mOptionsWidgetFactories << factory;
}

void QgisApp::unregisterOptionsWidgetFactory( QgsOptionsWidgetFactory *factory )
{
  mOptionsWidgetFactories.removeAll( factory );
}

void QgisApp::registerProjectPropertiesWidgetFactory( QgsOptionsWidgetFactory *factory )
{
  mProjectPropertiesWidgetFactories << factory;
}

void QgisApp::unregisterProjectPropertiesWidgetFactory( QgsOptionsWidgetFactory *factory )
{
  mProjectPropertiesWidgetFactories.removeAll( factory );
}

void QgisApp::registerDevToolFactory( QgsDevToolWidgetFactory *factory )
{
  mDevToolFactories << factory;
  if ( mDevToolsWidget )
  {
    // widget was already created, so we manually need to push this factory to the widget
    mDevToolsWidget->addToolFactory( factory );
  }
}

void QgisApp::unregisterDevToolFactory( QgsDevToolWidgetFactory *factory )
{
  mDevToolsWidget->removeToolFactory( factory );
  mDevToolFactories.removeAll( factory );
}

void QgisApp::registerApplicationExitBlocker( QgsApplicationExitBlockerInterface *blocker )
{
  mApplicationExitBlockers << blocker;
}

void QgisApp::unregisterApplicationExitBlocker( QgsApplicationExitBlockerInterface *blocker )
{
  mApplicationExitBlockers.removeAll( blocker );
}

void QgisApp::registerMapToolHandler( QgsAbstractMapToolHandler *handler )
{
  if ( !handler->action() || !handler->mapTool() )
  {
    QgsMessageLog::logMessage( tr( "Map tool handler is not properly constructed" ) );
    return;
  }

  mMapToolHandlers << handler;

  // do setup work
  handler->action()->setCheckable( true );
  handler->mapTool()->setAction( handler->action() );

  connect( handler->action(), &QAction::triggered, this, &QgisApp::switchToMapToolViaHandler );
  mMapToolGroup->addAction( handler->action() );
  QgsAbstractMapToolHandler::Context context;
  handler->action()->setEnabled( handler->isCompatibleWithLayer( activeLayer(), context ) );
}

void QgisApp::switchToMapToolViaHandler()
{
  QAction *sourceAction = qobject_cast< QAction * >( sender() );
  if ( !sourceAction )
    return;

  QgsAbstractMapToolHandler *handler = nullptr;
  for ( QgsAbstractMapToolHandler *h : std::as_const( mMapToolHandlers ) )
  {
    if ( h->action() == sourceAction )
    {
      handler = h;
      break;
    }
  }

  if ( !handler )
    return;

  if ( mMapCanvas->mapTool() == handler->mapTool() )
    return; // nothing to do

  handler->setLayerForTool( activeLayer() );
  mMapCanvas->setMapTool( handler->mapTool() );
}

void QgisApp::unregisterMapToolHandler( QgsAbstractMapToolHandler *handler )
{
  mMapToolHandlers.removeAll( handler );

  if ( !handler->action() || !handler->mapTool() )
  {
    return;
  }

  mMapToolGroup->removeAction( handler->action() );
  disconnect( handler->action(), &QAction::triggered, this, &QgisApp::switchToMapToolViaHandler );
}

QgsMapLayer *QgisApp::activeLayer()
{
  return mLayerTreeView ? mLayerTreeView->currentLayer() : nullptr;
}

QSize QgisApp::iconSize( bool dockedToolbar ) const
{
  return QgsGuiUtils::iconSize( dockedToolbar );
}

bool QgisApp::setActiveLayer( QgsMapLayer *layer )
{
  if ( !layer )
    return false;

  if ( !mLayerTreeView->layerTreeModel()->rootGroup()->findLayer( layer->id() ) )
    return false;

  mLayerTreeView->setCurrentLayer( layer );
  return true;
}

void QgisApp::reloadConnections()
{
  emit connectionsChanged( );
}

void QgisApp::showLayoutManager()
{
  static_cast< QgsAppWindowManager * >( QgsGui::windowManager() )->openApplicationDialog( QgsAppWindowManager::DialogLayoutManager );
}

void QgisApp::show3DMapViewsManager()
{
#ifdef HAVE_3D
  static_cast< QgsAppWindowManager * >( QgsGui::windowManager() )->openApplicationDialog( QgsAppWindowManager::Dialog3DMapViewsManager );
#endif
}

QgsVectorLayer *QgisApp::addVectorLayer( const QString &vectorLayerPath, const QString &name, const QString &providerKey )
{
  return addLayerPrivate< QgsVectorLayer >( QgsMapLayerType::VectorLayer, vectorLayerPath, name, !providerKey.isEmpty() ? providerKey : QLatin1String( "ogr" ), true );
}

template<typename T>
T *QgisApp::addLayerPrivate( QgsMapLayerType type, const QString &uri, const QString &name, const QString &providerKey, bool guiWarnings )
{
  QgsSettings settings;

  QgsCanvasRefreshBlocker refreshBlocker;

  QString baseName = settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() ? QgsMapLayer::formatLayerName( name ) : name;

  // if the layer needs authentication, ensure the master password is set
  const QRegularExpression rx( "authcfg=([a-z]|[A-Z]|[0-9]){7}" );
  if ( rx.match( uri ).hasMatch() )
  {
    if ( !QgsAuthGuiUtils::isDisabled( messageBar() ) )
    {
      QgsApplication::authManager()->setMasterPassword( true );
    }
  }

  QVariantMap uriElements = QgsProviderRegistry::instance()->decodeUri( providerKey, uri );
  QString path = uri;
  if ( uriElements.contains( QStringLiteral( "path" ) ) )
  {
    // run layer path through QgsPathResolver so that all inbuilt paths and other localised paths are correctly expanded
    path = QgsPathResolver().readPath( uriElements.value( QStringLiteral( "path" ) ).toString() );
    uriElements[ QStringLiteral( "path" ) ] = path;
  }
  // Not all providers implement decodeUri(), so use original uri if uriElements is empty
  const QString updatedUri = uriElements.isEmpty() ? uri : QgsProviderRegistry::instance()->encodeUri( providerKey, uriElements );

  const bool canQuerySublayers = QgsProviderRegistry::instance()->providerMetadata( providerKey ) &&
                                 ( QgsProviderRegistry::instance()->providerMetadata( providerKey )->capabilities() & QgsProviderMetadata::QuerySublayers );

  T *result = nullptr;
  if ( canQuerySublayers )
  {
    // query sublayers
    QList< QgsProviderSublayerDetails > sublayers = QgsProviderRegistry::instance()->providerMetadata( providerKey ) ?
        QgsProviderRegistry::instance()->providerMetadata( providerKey )->querySublayers( updatedUri, Qgis::SublayerQueryFlag::IncludeSystemTables )
        : QgsProviderRegistry::instance()->querySublayers( updatedUri );

    // filter out non-matching sublayers
    sublayers.erase( std::remove_if( sublayers.begin(), sublayers.end(), [type]( const QgsProviderSublayerDetails & sublayer )
    {
      return sublayer.type() != type;
    } ), sublayers.end() );

    if ( sublayers.empty() )
    {
      if ( guiWarnings )
      {
        QString msg = tr( "%1 is not a valid or recognized data source." ).arg( uri );
        visibleMessageBar()->pushMessage( tr( "Invalid Data Source" ), msg, Qgis::MessageLevel::Critical );
      }

      // since the layer is bad, stomp on it
      return nullptr;
    }
// SG change comment out lines below to load all sublayers without a dialog
    else if ( sublayers.size() > 1 || QgsProviderUtils::sublayerDetailsAreIncomplete( sublayers, QgsProviderUtils::SublayerCompletenessFlag::IgnoreUnknownFeatureCount ) )
    {
      // ask user for sublayers (unless user settings dictate otherwise!)
//      switch ( shouldAskUserForSublayers( sublayers ) )
//      {
//        case SublayerHandling::AskUser:
//        {
//          QgsProviderSublayersDialog dlg( updatedUri, path, sublayers, {type}, this );
//          if ( dlg.exec() )
//          {
//            const QList< QgsProviderSublayerDetails > selectedLayers = dlg.selectedLayers();
//            if ( !selectedLayers.isEmpty() )
//            {
//              result = qobject_cast< T * >( addSublayers( selectedLayers, baseName, dlg.groupName() ).value( 0 ) );
//            }
//          }
//          break;
//        }
//        case SublayerHandling::LoadAll:
//        {
          result = qobject_cast< T * >( addSublayers( sublayers, baseName, QString() ).value( 0 ) );
//          break;
//        }
//        case SublayerHandling::AbortLoading:
//          break;
//      };
    }
    else
    {
      result = qobject_cast< T * >( addSublayers( sublayers, name, QString() ).value( 0 ) );

      if ( result )
      {
        QString base( baseName );
        if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
        {
          base = QgsMapLayer::formatLayerName( base );
        }
        result->setName( base );
      }
    }
  }
  else
  {
    QgsMapLayerFactory::LayerOptions options( QgsProject::instance()->transformContext() );
    options.loadDefaultStyle = false;
    result = qobject_cast< T * >( QgsMapLayerFactory::createLayer( uri, name, type, options, providerKey ) );
    if ( result )
    {
      QString base( baseName );
      if ( settings.value( QStringLiteral( "qgis/formatLayerName" ), false ).toBool() )
      {
        base = QgsMapLayer::formatLayerName( base );
      }
      result->setName( base );
      QgsProject::instance()->addMapLayer( result );

      askUserForDatumTransform( result->crs(), QgsProject::instance()->crs(), result );
      postProcessAddedLayer( result );
    }
  }

  activateDeactivateLayerRelatedActions( activeLayer() );
  return result;
}

void QgisApp::addMapLayer( QgsMapLayer *mapLayer )
{
  QgsCanvasRefreshBlocker refreshBlocker;

  if ( mapLayer->isValid() )
  {
    // Register this layer with the layers registry
    QList<QgsMapLayer *> myList;
    myList << mapLayer;
    QgsProject::instance()->addMapLayers( myList );

    askUserForDatumTransform( mapLayer->crs(), QgsProject::instance()->crs(), mapLayer );
  }
  else
  {
    QString msg = tr( "The layer is not a valid layer and can not be added to the map" );
    visibleMessageBar()->pushMessage( tr( "Layer is not valid" ), msg, Qgis::MessageLevel::Critical );
  }
}


void QgisApp::embedLayers()
{
  //dialog to select groups/layers from other project files
  QgsProjectLayerGroupDialog d( this );
  if ( d.exec() == QDialog::Accepted && d.isValid() )
  {
    addEmbeddedItems( d.selectedProjectFile(), d.selectedGroups(), d.selectedLayerIds() );
  }
}

void QgisApp::addEmbeddedItems( const QString &projectFile, const QStringList &groups, const QStringList &layerIds )
{
  QgsCanvasRefreshBlocker refreshBlocker;

  //groups
  QStringList::const_iterator groupIt = groups.constBegin();
  for ( ; groupIt != groups.constEnd(); ++groupIt )
  {
    QgsLayerTreeGroup *newGroup = QgsProject::instance()->createEmbeddedGroup( *groupIt, projectFile, QStringList() );

    if ( newGroup )
      QgsProject::instance()->layerTreeRoot()->addChildNode( newGroup );
  }

  //layer ids
  QList<QDomNode> brokenNodes;

  // resolve dependencies
  QgsLayerDefinition::DependencySorter depSorter( projectFile );
  QStringList sortedIds = depSorter.sortedLayerIds();
  const auto constSortedIds = sortedIds;
  for ( const QString &id : constSortedIds )
  {
    const auto constLayerIds = layerIds;
    for ( const QString &selId : constLayerIds )
    {
      if ( selId == id )
        QgsProject::instance()->createEmbeddedLayer( selId, projectFile, brokenNodes );
    }
  }

  // fix broken relations and dependencies
  for ( const QString &id : constSortedIds )
  {
    QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( QgsProject::instance()->mapLayer( id ) );
    if ( vlayer )
      vectorLayerStyleLoaded( vlayer, QgsMapLayer::AllStyleCategories );
  }

  // Resolve references to other layers
  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( QMap<QString, QgsMapLayer *>::const_iterator it = layers.constBegin(); it != layers.constEnd(); ++it )
  {
    it.value()->resolveReferences( QgsProject::instance() );
  }
}

void QgisApp::newMapCanvas()
{
  int i = 1;

  bool existing = true;
  QList< QgsMapCanvas * > existingCanvases = mapCanvases();
  QString name;
  while ( existing )
  {
    name = tr( "Map %1" ).arg( i++ );
    existing = false;
    const auto constExistingCanvases = existingCanvases;
    for ( QgsMapCanvas *canvas : constExistingCanvases )
    {
      if ( canvas->objectName() == name )
      {
        existing = true;
        break;
      }
    }
  }

  QgsMapCanvasDockWidget *dock = createNewMapCanvasDock( name );
  if ( dock )
  {
    setupDockWidget( dock, true );
    dock->mapCanvas()->setLayers( mMapCanvas->layers() );
    dock->mapCanvas()->setExtent( mMapCanvas->extent() );
    QgsDebugMsgLevel( QStringLiteral( "QgisApp::newMapCanvas() -4- : QgsProject::instance()->crs().description[%1] ellipsoid[%2]" ).arg( QgsProject::instance()->crs().description(), QgsProject::instance()->crs().ellipsoidAcronym() ), 3 );
    dock->mapCanvas()->setDestinationCrs( QgsProject::instance()->crs() );
    dock->mapCanvas()->freeze( false );
  }
}

void QgisApp::init3D()
{
#ifdef HAVE_3D
  // initialize 3D registries
  Qgs3D::initialize();
  Qgs3DAppUtils::initialize();
#else
  m3DMapViewsMenu->menuAction()->setVisible( false );
#endif
}

void QgisApp::initNativeProcessing()
{
  QgsApplication::processingRegistry()->addProvider( new QgsNativeAlgorithms( QgsApplication::processingRegistry() ) );
#ifdef HAVE_3D
  QgsApplication::processingRegistry()->addProvider( new Qgs3DAlgorithms( QgsApplication::processingRegistry() ) );
#endif
}

void QgisApp::initLayouts()
{
  QgsLayoutGuiUtils::registerGuiForKnownItemTypes( mMapCanvas );

  // 3D map item
#ifdef HAVE_3D
  QgsApplication::layoutItemRegistry()->addLayoutItemType(
    new QgsLayoutItemMetadata( QgsLayoutItemRegistry::Layout3DMap, tr( "3D Map" ), tr( "3D Maps" ), QgsLayoutItem3DMap::create )
  );

  auto createRubberBand = ( []( QgsLayoutView * view )->QgsLayoutViewRubberBand *
  {
    return new QgsLayoutViewRectangularRubberBand( view );
  } );
  std::unique_ptr< QgsLayoutItemGuiMetadata > map3dMetadata = std::make_unique< QgsLayoutItemGuiMetadata>(
        QgsLayoutItemRegistry::Layout3DMap, tr( "3D Map" ), QgsApplication::getThemeIcon( QStringLiteral( "/mActionAdd3DMap.svg" ) ),
        [ = ]( QgsLayoutItem * item )->QgsLayoutItemBaseWidget *
  {
    return new QgsLayout3DMapWidget( qobject_cast< QgsLayoutItem3DMap * >( item ) );
  }, createRubberBand );
  QgsGui::layoutItemGuiRegistry()->addLayoutItemGuiMetadata( map3dMetadata.release() );
#endif

  mLayoutQptDropHandler = new QgsLayoutQptDropHandler( this );
  registerCustomLayoutDropHandler( mLayoutQptDropHandler );
  mLayoutImageDropHandler = new QgsLayoutImageDropHandler( this );
  registerCustomLayoutDropHandler( mLayoutImageDropHandler );
}

Qgs3DMapCanvasWidget *QgisApp::createNew3DMapCanvasDock( const QString &name, bool isDocked )
{
#ifdef HAVE_3D
  for ( Qgs3DMapCanvasWidget *canvas : mOpen3DMapViews )
  {
    if ( canvas->canvasName() == name )
    {
      QgsDebugMsg( QStringLiteral( "A map canvas with name '%1' already exists!" ).arg( name ) );
      return nullptr;
    }
  }

  markDirty();

  Qgs3DMapCanvasWidget *widget = new Qgs3DMapCanvasWidget( name, isDocked );

  mOpen3DMapViews.insert( widget );
  widget->setMainCanvas( mMapCanvas );
  widget->mapCanvas3D()->setTemporalController( mTemporalControllerWidget->temporalController() );

  return widget;
#else
  Q_UNUSED( name );
  Q_UNUSED( isDocked );
  return nullptr;
#endif
}

QgsElevationProfileWidget *QgisApp::createNewElevationProfile()
{
  QgsElevationProfileWidget *widget = new QgsElevationProfileWidget( tr( "Elevation Profile" ) );
  widget->setMainCanvas( mMapCanvas );
  return widget;
}

void QgisApp::new3DMapCanvas()
{
#ifdef HAVE_3D
  // initialize from project
  QgsRectangle fullExtent = mMapCanvas->projectExtent();

  // some layers may go crazy and make full extent unusable
  // we can't go any further - invalid extent would break everything
  if ( fullExtent.isEmpty() || !fullExtent.isFinite() )
  {
    QMessageBox::warning( this, tr( "New 3D Map View" ), tr( "Project extent is not valid. Please add or activate a layer to render." ) );
    return;
  }

  if ( mMapCanvas->mapSettings().destinationCrs().isGeographic() )
  {
    QMessageBox::warning( this, tr( "New 3D Map View" ), tr( "3D view currently does not support unprojected coordinate reference systems (CRS).\nPlease switch project's CRS to a projected CRS." ) );
    return;
  }

  int i = 1;
  const QList< QString > usedCanvasNames = QgsProject::instance()->viewsManager()->get3DViewsNames();
  QString name = tr( "3D Map %1" ).arg( i );
  while ( usedCanvasNames.contains( name ) )
  {
    name = tr( "3D Map %1" ).arg( ++i );
  }

  Qgs3DMapCanvasWidget *canvasWidget = createNew3DMapCanvasDock( name, true );
  if ( canvasWidget )
  {
    QgsProject *prj = QgsProject::instance();
    QgsRectangle fullExtent = mMapCanvas->projectExtent();
    QgsSettings settings;

    Qgs3DMapSettings *map = new Qgs3DMapSettings;
    map->setCrs( prj->crs() );
    map->setOrigin( QgsVector3D( fullExtent.center().x(), fullExtent.center().y(), 0 ) );
    map->setSelectionColor( mMapCanvas->selectionColor() );
    map->setBackgroundColor( mMapCanvas->canvasColor() );
    map->setLayers( mMapCanvas->layers( true ) );
    map->setTemporalRange( mMapCanvas->temporalRange() );

    const QgsCameraController::NavigationMode defaultNavMode = settings.enumValue( QStringLiteral( "map3d/defaultNavigation" ), QgsCameraController::TerrainBasedNavigation, QgsSettings::App );
    map->setCameraNavigationMode( defaultNavMode );

    map->setCameraMovementSpeed( settings.value( QStringLiteral( "map3d/defaultMovementSpeed" ), 5, QgsSettings::App ).toDouble() );
    const Qt3DRender::QCameraLens::ProjectionType defaultProjection = settings.enumValue( QStringLiteral( "map3d/defaultProjection" ), Qt3DRender::QCameraLens::PerspectiveProjection, QgsSettings::App );
    map->setProjectionType( defaultProjection );
    map->setFieldOfView( settings.value( QStringLiteral( "map3d/defaultFieldOfView" ), 45, QgsSettings::App ).toInt() );

    map->setTransformContext( QgsProject::instance()->transformContext() );
    map->setPathResolver( QgsProject::instance()->pathResolver() );
    map->setMapThemeCollection( QgsProject::instance()->mapThemeCollection() );

    map->configureTerrainFromProject( QgsProject::instance()->elevationProperties(), fullExtent );

    // new scenes default to a single directional light
    map->setDirectionalLights( QList<QgsDirectionalLightSettings>() << QgsDirectionalLightSettings() );
    map->setOutputDpi( QgsApplication::desktop()->logicalDpiX() );
    map->setRendererUsage( Qgis::RendererUsage::View );

    connect( QgsProject::instance(), &QgsProject::transformContextChanged, map, [map]
    {
      map->setTransformContext( QgsProject::instance()->transformContext() );
    } );

    canvasWidget->setMapSettings( map );

    QgsRectangle extent = mMapCanvas->extent();
    float dist = static_cast< float >( std::max( extent.width(), extent.height() ) );
    canvasWidget->mapCanvas3D()->setViewFromTop( mMapCanvas->extent().center(), dist, static_cast< float >( mMapCanvas->rotation() ) );

    const QgsCameraController::VerticalAxisInversion axisInversion = settings.enumValue( QStringLiteral( "map3d/axisInversion" ), QgsCameraController::WhenDragging, QgsSettings::App );
    if ( canvasWidget->mapCanvas3D()->cameraController() )
      canvasWidget->mapCanvas3D()->cameraController()->setVerticalAxisInversion( axisInversion );

    QDomImplementation DomImplementation;
    QDomDocumentType documentType =
      DomImplementation.createDocumentType(
        QStringLiteral( "qgis" ), QStringLiteral( "http://mrcc.com/qgis.dtd" ), QStringLiteral( "SYSTEM" ) );
    QDomDocument doc( documentType );

    QDomElement elem3DMap = doc.createElement( QStringLiteral( "view" ) );
    elem3DMap.setAttribute( QStringLiteral( "isOpen" ), 1 );

    write3DMapViewSettings( canvasWidget, doc, elem3DMap );

    QgsProject::instance()->viewsManager()->register3DViewSettings( name, elem3DMap );
    QgsProject::instance()->viewsManager()->set3DViewInitiallyVisible( name, true );
  }
#endif
}

void QgisApp::setExtent( const QgsRectangle &rect )
{
  mMapCanvas->setExtent( rect );
}

bool QgisApp::saveDirty()
{
  QString whyDirty;
  bool hasUnsavedEdits = false;
  // extra check to see if there are any vector layers with unsaved provider edits
  // to ensure user has opportunity to save any editing
  if ( QgsProject::instance()->count() > 0 )
  {
    QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
    for ( QMap<QString, QgsMapLayer *>::iterator it = layers.begin(); it != layers.end(); ++it )
    {
      QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( it.value() );
      // note that we skip the unsaved edits check for memory layers -- it's misleading, because their contents aren't actually
      // saved if this is part of a project close operation. Instead we let these get picked up by checkMemoryLayers().
      if ( !vl || vl->providerType() == QLatin1String( "memory" ) )
      {
        continue;
      }

      hasUnsavedEdits = ( vl->isEditable() && vl->isModified() );
      if ( hasUnsavedEdits )
      {
        break;
      }
    }

    if ( hasUnsavedEdits )
    {
      markDirty();
      whyDirty = QStringLiteral( "<p style='color:darkred;'>" );
      whyDirty += tr( "Project has layer(s) in edit mode with unsaved edits, which will NOT be saved!" );
      whyDirty += QLatin1String( "</p>" );
    }
  }

  QMessageBox::StandardButton answer( QMessageBox::Discard );
  QgsCanvasRefreshBlocker refreshBlocker;

  QgsSettings settings;
  bool askThem = settings.value( QStringLiteral( "qgis/askToSaveProjectChanges" ), true ).toBool();

  if ( askThem && QgsProject::instance()->isDirty() )
  {
    // flag project as dirty since dirty state of canvas is reset if "dirty"
    // is based on a zoom or pan
    markDirty();

    // prompt user to save
    answer = QMessageBox::question( this, tr( "Save Project" ),
                                    tr( "Do you want to save the current project? %1" )
                                    .arg( whyDirty ),
                                    QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard,
                                    hasUnsavedEdits ? QMessageBox::Cancel : QMessageBox::Save );
    if ( QMessageBox::Save == answer )
    {
      if ( !fileSave() )
        answer = QMessageBox::Cancel;
    }
  }

  if ( answer == QMessageBox::Cancel )
    return false;

  // for memory layers, we discard all unsaved changes manually. Users have already been warned about
  // these by an earlier call to checkMemoryLayers(), and we don't want duplicate "unsaved changes" prompts
  // and anyway, saving the changes to a memory layer here won't actually save ANYTHING!
  // we do this at the very end here, because if the user opted to cancel above then ALL unsaved
  // changes in memory layers should still exist for them.
  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( auto it = layers.begin(); it != layers.end(); ++it )
  {
    if ( QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( it.value() ) )
    {
      if ( vl->providerType() == QLatin1String( "memory" ) && vl->isEditable() && vl->isModified() )
      {
        vl->rollBack();
      }
    }
  }

  return true;
}

bool QgisApp::checkUnsavedLayerEdits()
{
  // check to see if there are any vector layers with unsaved provider edits
  // to ensure user has opportunity to save any editing
  if ( QgsProject::instance()->count() > 0 )
  {
    const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
    for ( auto it = layers.begin(); it != layers.end(); ++it )
    {
      if ( QgsVectorLayer *vl = qobject_cast<QgsVectorLayer *>( it.value() ) )
      {
        // note that we skip the unsaved edits check for memory layers -- it's misleading, because their contents aren't actually
        // saved if this is part of a project close operation. Instead we let these get picked up by checkMemoryLayers()
        if ( ! vl->dataProvider() || vl->providerType() == QLatin1String( "memory" ) )
          continue;

        const bool hasUnsavedEdits = ( vl->isEditable() && vl->isModified() );
        if ( !hasUnsavedEdits )
          continue;

        if ( !toggleEditing( vl, true ) )
          return false;
      }
    }
  }

  return true;
}

bool QgisApp::checkMemoryLayers()
{
  if ( !QgsSettings().value( QStringLiteral( "askToSaveMemoryLayers" ), true, QgsSettings::App ).toBool() )
    return true;

  // check to see if there are any temporary layers present (with features)
  bool hasTemporaryLayers = false;
  bool hasMemoryLayers = false;

  const QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( auto it = layers.begin(); it != layers.end(); ++it )
  {
    if ( it.value() && it.value()->providerType() == QLatin1String( "memory" ) )
    {
      QgsVectorLayer *vl = qobject_cast< QgsVectorLayer * >( it.value() );
      if ( vl && vl->featureCount() != 0 && !vl->customProperty( QStringLiteral( "skipMemoryLayersCheck" ) ).toInt() )
      {
        hasMemoryLayers = true;
        break;
      }
    }
    else if ( it.value() && it.value()->isTemporary() )
    {
      hasTemporaryLayers = true;
    }
  }

  bool close = true;
  if ( hasTemporaryLayers )
    close &= QMessageBox::warning( this,
                                   tr( "Close Project" ),
                                   tr( "This project includes one or more temporary layers. These layers are not permanently saved and their contents will be lost. Are you sure you want to proceed?" ),
                                   QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel ) == QMessageBox::Yes ;
  else if ( hasMemoryLayers )
    // use the more specific warning for memory layers
    close &= QMessageBox::warning( this,
                                   tr( "Close Project" ),
                                   tr( "This project includes one or more temporary scratch layers. These layers are not saved to disk and their contents will be permanently lost. Are you sure you want to proceed?" ),
                                   QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Cancel ) == QMessageBox::Yes;

  return close;
}

bool QgisApp::checkExitBlockers()
{
  for ( QgsApplicationExitBlockerInterface *blocker : std::as_const( mApplicationExitBlockers ) )
  {
    if ( !blocker->allowExit() )
      return false;
  }
  return true;
}

bool QgisApp::checkTasksDependOnProject()
{
  QSet< QString > activeTaskDescriptions;
  QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  QMap<QString, QgsMapLayer *>::const_iterator layerIt = layers.constBegin();

  for ( ; layerIt != layers.constEnd(); ++layerIt )
  {
    QList< QgsTask * > tasks = QgsApplication::taskManager()->tasksDependentOnLayer( layerIt.value() );
    if ( !tasks.isEmpty() )
    {
      const auto constTasks = tasks;
      for ( QgsTask *task : constTasks )
      {
        activeTaskDescriptions.insert( tr( " • %1" ).arg( task->description() ) );
      }
    }
  }

  if ( !activeTaskDescriptions.isEmpty() )
  {
    QMessageBox::warning( this, tr( "Active Tasks" ),
                          tr( "The following tasks are currently running which depend on layers in this project:\n\n%1\n\nPlease cancel these tasks and retry." ).arg( qgis::setToList( activeTaskDescriptions ).join( QLatin1Char( '\n' ) ) ) );
    return true;
  }
  return false;
}

void QgisApp::closeProject()
{
  QgsCanvasRefreshBlocker refreshBlocker;

  // unload the project macros before changing anything
  if ( mPythonMacrosEnabled )
  {
    QgsPythonRunner::run( QStringLiteral( "qgis.utils.unloadProjectMacros();" ) );
  }

  mPythonMacrosEnabled = false;

  mLegendExpressionFilterButton->setExpressionText( QString() );
  mLegendExpressionFilterButton->setChecked( false );
  mFilterLegendByMapContentAction->setChecked( false );

  const QList<QgsElevationProfileWidget *> elevationProfileWidgets = findChildren< QgsElevationProfileWidget * >();
  for ( QgsElevationProfileWidget *widget : elevationProfileWidgets )
  {
    delete widget;
  }

  closeAdditionalMapCanvases();
  closeAdditional3DMapCanvases();

  deleteLayoutDesigners();

  // ensure layout widgets are fully deleted
  QgsApplication::sendPostedEvents( nullptr, QEvent::DeferredDelete );

  removeAnnotationItems();

  // clear out any stuff from project
  mMapCanvas->setLayers( QList<QgsMapLayer *>() );
  mMapCanvas->clearCache();
  mMapCanvas->cancelJobs();
  mOverviewCanvas->setLayers( QList<QgsMapLayer *>() );

  // Avoid unnecessary layer changed handling for each layer removed - instead,
  // defer the handling until we've removed all layers
  mBlockActiveLayerChanged = true;
  QgsProject::instance()->clear();
  mBlockActiveLayerChanged = false;

  onActiveLayerChanged( activeLayer() );
}


void QgisApp::changeEvent( QEvent *event )
{
  QMainWindow::changeEvent( event );
#ifdef Q_OS_MAC
  switch ( event->type() )
  {
    case QEvent::ActivationChange:
      if ( QApplication::activeWindow() == this )
      {
        mWindowAction->setChecked( true );
      }
      // this should not be necessary since the action is part of an action group
      // however this check is not cleared if PrintComposer is closed and reopened
      else
      {
        mWindowAction->setChecked( false );
      }
      break;

    case QEvent::WindowTitleChange:
      mWindowAction->setText( windowTitle() );
      break;

    default:
      break;
  }
#endif
}

void QgisApp::closeEvent( QCloseEvent *event )
{
  // We'll close in our own good time, thank you very much
  event->ignore();
  // Do the usual checks and ask if they want to save, etc
  fileExit();
}

QMenu *QgisApp::getPluginMenu( const QString &menuName )
{
  /* Plugin menu items are below the plugin separator (which may not exist yet
   * if no plugins are loaded) and above the python separator. If python is not
   * present, there is no python separator and the plugin list is at the bottom
   * of the menu.
   */

  QString cleanedMenuName = menuName;
#ifdef Q_OS_MAC
  // Mac doesn't have '&' keyboard shortcuts.
  cleanedMenuName.remove( QChar( '&' ) );
#endif
  QAction *before = mActionPluginSeparator2;  // python separator or end of list
  if ( !mActionPluginSeparator1 )
  {
    // First plugin - create plugin list separator
    mActionPluginSeparator1 = mPluginMenu->insertSeparator( before );
  }
  else
  {
    QString dst = cleanedMenuName;
    dst.remove( QChar( '&' ) );

    // Plugins exist - search between plugin separator and python separator or end of list
    QList<QAction *> actions = mPluginMenu->actions();
    int end = mActionPluginSeparator2 ? actions.indexOf( mActionPluginSeparator2 ) : actions.count();
    for ( int i = actions.indexOf( mActionPluginSeparator1 ) + 1; i < end; i++ )
    {
      QString src = actions.at( i )->text();
      src.remove( QChar( '&' ) );

      int comp = dst.localeAwareCompare( src );
      if ( comp < 0 )
      {
        // Add item before this one
        before = actions.at( i );
        break;
      }
      else if ( comp == 0 )
      {
        // Plugin menu item already exists
        return actions.at( i )->menu();
      }
    }
  }
  // It doesn't exist, so create
  QMenu *menu = new QMenu( cleanedMenuName, this );
  menu->setObjectName( normalizedMenuName( cleanedMenuName ) );
  // Where to put it? - we worked that out above...
  mPluginMenu->insertMenu( before, menu );

  return menu;
}

void QgisApp::addPluginToMenu( const QString &name, QAction *action )
{
  QMenu *menu = getPluginMenu( name );
  menu->addAction( action );
}

void QgisApp::removePluginMenu( const QString &name, QAction *action )
{
  QMenu *menu = getPluginMenu( name );
  menu->removeAction( action );
  if ( menu->actions().isEmpty() )
  {
    mPluginMenu->removeAction( menu->menuAction() );
  }
  // Remove separator above plugins in Plugin menu if no plugins remain
  QList<QAction *> actions = mPluginMenu->actions();
  int end = mActionPluginSeparator2 ? actions.indexOf( mActionPluginSeparator2 ) : actions.count();
  if ( actions.indexOf( mActionPluginSeparator1 ) + 1 == end )
  {
    mPluginMenu->removeAction( mActionPluginSeparator1 );
    mActionPluginSeparator1 = nullptr;
  }
}

QMenu *QgisApp::getDatabaseMenu( const QString &menuName )
{
  if ( menuName.isEmpty() )
    return mDatabaseMenu;

  QString cleanedMenuName = menuName;
#ifdef Q_OS_MAC
  // Mac doesn't have '&' keyboard shortcuts.
  cleanedMenuName.remove( QChar( '&' ) );
#endif
  QString dst = cleanedMenuName;
  dst.remove( QChar( '&' ) );

  QAction *before = nullptr;
  QList<QAction *> actions = mDatabaseMenu->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    QString src = actions.at( i )->text();
    src.remove( QChar( '&' ) );

    int comp = dst.localeAwareCompare( src );
    if ( comp < 0 )
    {
      // Add item before this one
      before = actions.at( i );
      break;
    }
    else if ( comp == 0 )
    {
      // Plugin menu item already exists
      return actions.at( i )->menu();
    }
  }
  // It doesn't exist, so create
  QMenu *menu = new QMenu( cleanedMenuName, this );
  menu->setObjectName( normalizedMenuName( cleanedMenuName ) );
  if ( before )
    mDatabaseMenu->insertMenu( before, menu );
  else
    mDatabaseMenu->addMenu( menu );

  return menu;
}

QMenu *QgisApp::getRasterMenu( const QString &menuName )
{
  if ( menuName.isEmpty() )
    return mRasterMenu;

  QString cleanedMenuName = menuName;
#ifdef Q_OS_MAC
  // Mac doesn't have '&' keyboard shortcuts.
  cleanedMenuName.remove( QChar( '&' ) );
#endif

  QAction *before = nullptr;
  if ( !mActionRasterSeparator )
  {
    // First plugin - create plugin list separator
    mActionRasterSeparator = mRasterMenu->insertSeparator( before );
  }
  else
  {
    QString dst = cleanedMenuName;
    dst.remove( QChar( '&' ) );
    // Plugins exist - search between plugin separator and python separator or end of list
    QList<QAction *> actions = mRasterMenu->actions();
    for ( int i = actions.indexOf( mActionRasterSeparator ) + 1; i < actions.count(); i++ )
    {
      QString src = actions.at( i )->text();
      src.remove( QChar( '&' ) );

      int comp = dst.localeAwareCompare( src );
      if ( comp < 0 )
      {
        // Add item before this one
        before = actions.at( i );
        break;
      }
      else if ( comp == 0 )
      {
        // Plugin menu item already exists
        return actions.at( i )->menu();
      }
    }
  }

  // It doesn't exist, so create
  QMenu *menu = new QMenu( cleanedMenuName, this );
  menu->setObjectName( normalizedMenuName( cleanedMenuName ) );
  if ( before )
    mRasterMenu->insertMenu( before, menu );
  else
    mRasterMenu->addMenu( menu );

  return menu;
}

QMenu *QgisApp::getVectorMenu( const QString &menuName )
{
  if ( menuName.isEmpty() )
    return mVectorMenu;

  QString cleanedMenuName = menuName;
#ifdef Q_OS_MAC
  // Mac doesn't have '&' keyboard shortcuts.
  cleanedMenuName.remove( QChar( '&' ) );
#endif
  QString dst = cleanedMenuName;
  dst.remove( QChar( '&' ) );

  QAction *before = nullptr;
  QList<QAction *> actions = mVectorMenu->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    QString src = actions.at( i )->text();
    src.remove( QChar( '&' ) );

    int comp = dst.localeAwareCompare( src );
    if ( comp < 0 )
    {
      // Add item before this one
      before = actions.at( i );
      break;
    }
    else if ( comp == 0 )
    {
      // Plugin menu item already exists
      return actions.at( i )->menu();
    }
  }
  // It doesn't exist, so create
  QMenu *menu = new QMenu( cleanedMenuName, this );
  menu->setObjectName( normalizedMenuName( cleanedMenuName ) );
  if ( before )
    mVectorMenu->insertMenu( before, menu );
  else
    mVectorMenu->addMenu( menu );

  return menu;
}

QMenu *QgisApp::getWebMenu( const QString &menuName )
{
  if ( menuName.isEmpty() )
    return mWebMenu;

  QString cleanedMenuName = menuName;
#ifdef Q_OS_MAC
  // Mac doesn't have '&' keyboard shortcuts.
  cleanedMenuName.remove( QChar( '&' ) );
#endif
  QString dst = cleanedMenuName;
  dst.remove( QChar( '&' ) );

  QAction *before = nullptr;
  QList<QAction *> actions = mWebMenu->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    QString src = actions.at( i )->text();
    src.remove( QChar( '&' ) );

    int comp = dst.localeAwareCompare( src );
    if ( comp < 0 )
    {
      // Add item before this one
      before = actions.at( i );
      break;
    }
    else if ( comp == 0 )
    {
      // Plugin menu item already exists
      return actions.at( i )->menu();
    }
  }
  // It doesn't exist, so create
  QMenu *menu = new QMenu( cleanedMenuName, this );
  menu->setObjectName( normalizedMenuName( cleanedMenuName ) );
  if ( before )
    mWebMenu->insertMenu( before, menu );
  else
    mWebMenu->addMenu( menu );

  return menu;
}

QMenu *QgisApp::getMeshMenu( const QString &menuName )
{
  if ( menuName.isEmpty() )
    return mMeshMenu;

  QString cleanedMenuName = menuName;
#ifdef Q_OS_MAC
  // Mac doesn't have '&' keyboard shortcuts.
  cleanedMenuName.remove( QChar( '&' ) );
#endif
  QString dst = cleanedMenuName;
  dst.remove( QChar( '&' ) );

  QAction *before = nullptr;
  QList<QAction *> actions = mMeshMenu->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    QString src = actions.at( i )->text();
    src.remove( QChar( '&' ) );

    int comp = dst.localeAwareCompare( src );
    if ( comp < 0 )
    {
      // Add item before this one
      before = actions.at( i );
      break;
    }
    else if ( comp == 0 )
    {
      // Plugin menu item already exists
      return actions.at( i )->menu();
    }
  }
  // It doesn't exist, so create
  QMenu *menu = new QMenu( cleanedMenuName, this );
  menu->setObjectName( normalizedMenuName( cleanedMenuName ) );
  if ( before )
    mMeshMenu->insertMenu( before, menu );
  else
    mMeshMenu->addMenu( menu );

  return menu;
}

void QgisApp::insertAddLayerAction( QAction *action )
{
  mAddLayerMenu->insertAction( mActionAddLayerSeparator, action );
}

void QgisApp::removeAddLayerAction( QAction *action )
{
  mAddLayerMenu->removeAction( action );
}

void QgisApp::addPluginToDatabaseMenu( const QString &name, QAction *action )
{
  QMenu *menu = getDatabaseMenu( name );
  menu->addAction( action );

  // add the Database menu to the menuBar if not added yet
  if ( mDatabaseMenu->actions().count() != 1 )
    return;

  QAction *before = nullptr;
  QList<QAction *> actions = menuBar()->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    if ( actions.at( i )->menu() == mDatabaseMenu )
      return;

    // goes before Web menu, if present
    if ( actions.at( i )->menu() == mWebMenu )
    {
      before = actions.at( i );
      break;
    }
  }
  for ( int i = 0; i < actions.count(); i++ )
  {
    // defaults to after Raster menu, which is already in qgisapp.ui
    if ( actions.at( i )->menu() == mRasterMenu )
    {
      if ( !before )
      {
        before = actions.at( i += 1 );
        break;
      }
    }
  }
  if ( before )
    menuBar()->insertMenu( before, mDatabaseMenu );
  else
    // fallback insert
    menuBar()->insertMenu( firstRightStandardMenu()->menuAction(), mDatabaseMenu );
}

void QgisApp::addPluginToRasterMenu( const QString &name, QAction *action )
{
  QMenu *menu = getRasterMenu( name );
  menu->addAction( action );
}

void QgisApp::addPluginToVectorMenu( const QString &name, QAction *action )
{
  QMenu *menu = getVectorMenu( name );
  menu->addAction( action );
}

void QgisApp::addPluginToWebMenu( const QString &name, QAction *action )
{
  QMenu *menu = getWebMenu( name );
  menu->addAction( action );

  // add the Web menu to the menuBar if not added yet
  if ( mWebMenu->actions().count() != 1 )
    return;

  QAction *before = nullptr;
  QList<QAction *> actions = menuBar()->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    // goes after Database menu, if present
    if ( actions.at( i )->menu() == mDatabaseMenu )
    {
      before = actions.at( i += 1 );
      // don't break here
    }

    if ( actions.at( i )->menu() == mWebMenu )
      return;
  }
  for ( int i = 0; i < actions.count(); i++ )
  {
    // defaults to after Raster menu, which is already in qgisapp.ui
    if ( actions.at( i )->menu() == mRasterMenu )
    {
      if ( !before )
      {
        before = actions.at( i += 1 );
        break;
      }
    }
  }

  if ( before )
    menuBar()->insertMenu( before, mWebMenu );
  else
    // fallback insert
    menuBar()->insertMenu( firstRightStandardMenu()->menuAction(), mWebMenu );
}

void QgisApp::addPluginToMeshMenu( const QString &name, QAction *action )
{
  QMenu *menu = getMeshMenu( name );
  menu->addAction( action );
}

void QgisApp::removePluginDatabaseMenu( const QString &name, QAction *action )
{
  QMenu *menu = getDatabaseMenu( name );
  menu->removeAction( action );
  if ( menu->actions().isEmpty() )
  {
    mDatabaseMenu->removeAction( menu->menuAction() );
  }

  // remove the Database menu from the menuBar if there are no more actions
  if ( !mDatabaseMenu->actions().isEmpty() )
    return;

  QList<QAction *> actions = menuBar()->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    if ( actions.at( i )->menu() == mDatabaseMenu )
    {
      menuBar()->removeAction( actions.at( i ) );
      return;
    }
  }
}

void QgisApp::removePluginRasterMenu( const QString &name, QAction *action )
{
  QMenu *menu = getRasterMenu( name );
  menu->removeAction( action );
  if ( menu->actions().isEmpty() )
  {
    mRasterMenu->removeAction( menu->menuAction() );
  }

  // Remove separator above plugins in Raster menu if no plugins remain
  QList<QAction *> actions = mRasterMenu->actions();
  if ( actions.indexOf( mActionRasterSeparator ) + 1 == actions.count() )
  {
    mRasterMenu->removeAction( mActionRasterSeparator );
    mActionRasterSeparator = nullptr;
  }
}

void QgisApp::removePluginVectorMenu( const QString &name, QAction *action )
{
  QMenu *menu = getVectorMenu( name );
  menu->removeAction( action );
  if ( menu->actions().isEmpty() )
  {
    mVectorMenu->removeAction( menu->menuAction() );
  }

  // remove the Vector menu from the menuBar if there are no more actions
  if ( !mVectorMenu->actions().isEmpty() )
    return;

  QList<QAction *> actions = menuBar()->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    if ( actions.at( i )->menu() == mVectorMenu )
    {
      menuBar()->removeAction( actions.at( i ) );
      return;
    }
  }
}

void QgisApp::removePluginWebMenu( const QString &name, QAction *action )
{
  QMenu *menu = getWebMenu( name );
  menu->removeAction( action );
  if ( menu->actions().isEmpty() )
  {
    mWebMenu->removeAction( menu->menuAction() );
  }

  // remove the Web menu from the menuBar if there are no more actions
  if ( !mWebMenu->actions().isEmpty() )
    return;

  QList<QAction *> actions = menuBar()->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    if ( actions.at( i )->menu() == mWebMenu )
    {
      menuBar()->removeAction( actions.at( i ) );
      return;
    }
  }
}

void QgisApp::removePluginMeshMenu( const QString &name, QAction *action )
{
  QMenu *menu = getMeshMenu( name );
  menu->removeAction( action );
  if ( menu->actions().isEmpty() )
  {
    mMeshMenu->removeAction( menu->menuAction() );
  }

  // remove the Mesh menu from the menuBar if there are no more actions
  if ( !mMeshMenu->actions().isEmpty() )
    return;

  QList<QAction *> actions = menuBar()->actions();
  for ( int i = 0; i < actions.count(); i++ )
  {
    if ( actions.at( i )->menu() == mMeshMenu )
    {
      menuBar()->removeAction( actions.at( i ) );
      return;
    }
  }
}

int QgisApp::addPluginToolBarIcon( QAction *qAction )
{
  mPluginToolBar->addAction( qAction );
  return 0;
}

QAction *QgisApp::addPluginToolBarWidget( QWidget *widget )
{
  return mPluginToolBar->addWidget( widget );
}

void QgisApp::removePluginToolBarIcon( QAction *qAction )
{
  mPluginToolBar->removeAction( qAction );
}

int QgisApp::addRasterToolBarIcon( QAction *qAction )
{
  mRasterToolBar->addAction( qAction );
  return 0;
}

QAction *QgisApp::addRasterToolBarWidget( QWidget *widget )
{
  return mRasterToolBar->addWidget( widget );
}

void QgisApp::removeRasterToolBarIcon( QAction *qAction )
{
  mRasterToolBar->removeAction( qAction );
}

int QgisApp::addVectorToolBarIcon( QAction *qAction )
{
  mVectorToolBar->addAction( qAction );
  return 0;
}

QAction *QgisApp::addVectorToolBarWidget( QWidget *widget )
{
  return mVectorToolBar->addWidget( widget );
}

void QgisApp::removeVectorToolBarIcon( QAction *qAction )
{
  mVectorToolBar->removeAction( qAction );
}

int QgisApp::addDatabaseToolBarIcon( QAction *qAction )
{
  mDatabaseToolBar->addAction( qAction );
  return 0;
}

void QgisApp::onVirtualLayerAdded( const QString &uri, const QString &layerName )
{
  addVectorLayer( uri, layerName, QStringLiteral( "virtual" ) );
}

QAction *QgisApp::addDatabaseToolBarWidget( QWidget *widget )
{
  return mDatabaseToolBar->addWidget( widget );
}

void QgisApp::removeDatabaseToolBarIcon( QAction *qAction )
{
  mDatabaseToolBar->removeAction( qAction );
}

int QgisApp::addWebToolBarIcon( QAction *qAction )
{
  mWebToolBar->addAction( qAction );
  return 0;
}

QAction *QgisApp::addWebToolBarWidget( QWidget *widget )
{
  return mWebToolBar->addWidget( widget );
}

void QgisApp::removeWebToolBarIcon( QAction *qAction )
{
  mWebToolBar->removeAction( qAction );
}

void QgisApp::updateCrsStatusBar()
{
  const QgsCoordinateReferenceSystem projectCrs = QgsProject::instance()->crs();
  if ( projectCrs.isValid() )
  {
    if ( !projectCrs.authid().isEmpty() )
      mOnTheFlyProjectionStatusButton->setText( projectCrs.authid() );
    else
      mOnTheFlyProjectionStatusButton->setText( tr( "Unknown CRS" ) );

    mOnTheFlyProjectionStatusButton->setToolTip(
      tr( "Current CRS: %1" ).arg( projectCrs.userFriendlyIdentifier() ) );
    mOnTheFlyProjectionStatusButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "mIconProjectionEnabled.svg" ) ) );
  }
  else
  {
    mOnTheFlyProjectionStatusButton->setText( QString() );
    mOnTheFlyProjectionStatusButton->setToolTip( tr( "No projection" ) );
    mOnTheFlyProjectionStatusButton->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "mIconProjectionDisabled.svg" ) ) );
  }
}

// slot to update the progress bar in the status bar
void QgisApp::showProgress( int progress, int totalSteps )
{
  if ( progress == totalSteps )
  {
    mProgressBar->reset();
    mProgressBar->hide();
  }
  else
  {
    //only call show if not already hidden to reduce flicker
    if ( !mProgressBar->isVisible() )
    {
      mProgressBar->show();
    }
    mProgressBar->setMaximum( totalSteps );
    mProgressBar->setValue( progress );
  }
}

void QgisApp::mapToolChanged( QgsMapTool *newTool, QgsMapTool *oldTool )
{
  if ( oldTool )
  {
    disconnect( oldTool, &QgsMapTool::messageEmitted, this, &QgisApp::displayMapToolMessage );
    disconnect( oldTool, &QgsMapTool::messageEmitted, this, &QgisApp::displayMapToolMessage );
    disconnect( oldTool, &QgsMapTool::messageDiscarded, this, &QgisApp::removeMapToolMessage );
  }

  if ( newTool )
  {
    if ( !( newTool->flags() & QgsMapTool::EditTool ) )
    {
      mNonEditMapTool = newTool;
    }

    connect( newTool, &QgsMapTool::messageEmitted, this, &QgisApp::displayMapToolMessage );
    connect( newTool, &QgsMapTool::messageEmitted, this, &QgisApp::displayMapToolMessage );
    connect( newTool, &QgsMapTool::messageDiscarded, this, &QgisApp::removeMapToolMessage );
  }
}

void QgisApp::showMapCanvas()
{
  // Map layers changed -> switch to map canvas
  if ( mCentralContainer )
    mCentralContainer->setCurrentIndex( 0 );
}

void QgisApp::markDirty()
{
  // notify the project that there was a change
  QgsProject::instance()->setDirty( true );
}

void QgisApp::extentChanged()
{
  // allow symbols in the legend update their preview if they use map units
  mLayerTreeView->layerTreeModel()->setLegendMapViewData( mMapCanvas->mapUnitsPerPixel(),
      static_cast< int >( std::round( mMapCanvas->mapSettings().outputDpi() ) ), mMapCanvas->scale() );
}

void QgisApp::layersWereAdded( const QList<QgsMapLayer *> &layers )
{
  const auto constLayers = layers;
  for ( QgsMapLayer *layer : constLayers )
  {
    connect( layer, &QgsMapLayer::layerModified, this, &QgisApp::updateLayerModifiedActions );
    connect( layer, &QgsMapLayer::editingStarted, this, &QgisApp::layerEditStateChanged );
    connect( layer, &QgsMapLayer::editingStopped, this, &QgisApp::layerEditStateChanged );

    if ( QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer ) )
    {
      // notify user about any font family substitution, but only when rendering labels (i.e. not when opening settings dialog)
      connect( vlayer, &QgsVectorLayer::labelingFontNotFound, this, &QgisApp::labelingFontNotFound );

      // Do not check for layer editing capabilities because they may change
      // (for example when subsetString is added/removed) and signals need to
      // be in place in order to update the GUI
      connect( vlayer, &QgsVectorLayer::readOnlyChanged, this, &QgisApp::layerEditStateChanged );
      connect( vlayer, &QgsVectorLayer::raiseError, this, &QgisApp::onLayerError );
      connect( vlayer, &QgsVectorLayer::styleLoaded, [this, vlayer]( QgsMapLayer::StyleCategories categories ) { vectorLayerStyleLoaded( vlayer, categories ); } );
    }

    if ( QgsRasterLayer *rlayer = qobject_cast<QgsRasterLayer *>( layer ) )
    {
      // connect up any request the raster may make to update the statusbar message
      connect( rlayer, &QgsRasterLayer::statusChanged, this, &QgisApp::showStatusMessage );
    }

    if ( QgsPointCloudLayer *pclayer = qobject_cast<QgsPointCloudLayer *>( layer ) )
    {
      connect( pclayer, &QgsPointCloudLayer::raiseError, this, &QgisApp::onLayerError );
    }

    if ( QgsDataProvider *provider = layer->dataProvider() )
    {
      connect( provider, &QgsDataProvider::dataChanged, layer, [layer] { layer->triggerRepaint(); } );
      connect( provider, &QgsDataProvider::dataChanged, this, [this] { refreshMapCanvas(); } );
    }
  }
}

void QgisApp::showRotation()
{
  // update the statusbar with the current rotation.
  double myrotation = mMapCanvas->rotation();
  whileBlocking( mRotationEdit )->setValue( myrotation );
}

void QgisApp::showPanMessage( double distance, QgsUnitTypes::DistanceUnit unit, double bearing )
{
    // SG change
    Q_UNUSED(distance);
    Q_UNUSED(unit);
    Q_UNUSED(bearing);

    return;

    //  const bool showMessage = QgsSettings().value( QStringLiteral( "showPanDistanceInStatusBar" ), true, QgsSettings::App ).toBool();
    //  if ( !showMessage )
    //    return;

    //  const double distanceInProjectUnits = distance * QgsUnitTypes::fromUnitToUnitFactor( unit, QgsProject::instance()->distanceUnits() );
    //  const int distanceDecimalPlaces = QgsSettings().value( QStringLiteral( "qgis/measure/decimalplaces" ), 3 ).toInt();
    //  const QString distanceString = QgsDistanceArea::formatDistance( distanceInProjectUnits, distanceDecimalPlaces, QgsProject::instance()->distanceUnits() );
    //  const QString bearingString = mBearingNumericFormat->formatDouble( bearing, QgsNumericFormatContext() );
    //  mStatusBar->showMessage( tr( "Pan distance %1 (%2)" ).arg( distanceString, bearingString ), 2000 );
}

void QgisApp::selectionModeChanged( QgsMapToolSelect::Mode mode )
{
  switch ( mode )
  {
    case QgsMapToolSelect::GeometryIntersectsSetSelection:
      mStatusBar->showMessage( QString() );
      break;
    case QgsMapToolSelect::GeometryIntersectsAddToSelection:
      mStatusBar->showMessage( tr( "Add to the current selection" ) );
      break;

    case QgsMapToolSelect::GeometryIntersectsSubtractFromSelection:
      mStatusBar->showMessage( tr( "Subtract from the current selection" ) );
      break;

    case QgsMapToolSelect::GeometryIntersectsIntersectWithSelection:
      mStatusBar->showMessage( tr( "Intersect with the current selection" ) );
      break;

    case QgsMapToolSelect::GeometryWithinSetSelection:
      mStatusBar->showMessage( tr( "Select features completely within" ) );
      break;

    case QgsMapToolSelect::GeometryWithinAddToSelection:
      mStatusBar->showMessage( tr( "Add features completely within to the current selection" ) );
      break;

    case QgsMapToolSelect::GeometryWithinSubtractFromSelection:
      mStatusBar->showMessage( tr( "Subtract features completely within from the current selection" ) );
      break;

    case QgsMapToolSelect::GeometryWithinIntersectWithSelection:
      mStatusBar->showMessage( tr( "Intersect features completely within with the current selection" ) );
      break;

  }
}

void QgisApp::updateMouseCoordinatePrecision()
{
    // SG change
    // mCoordsEdit->setMouseCoordinatesPrecision( QgsCoordinateUtils::calculateCoordinatePrecision( mapCanvas()->mapUnitsPerPixel(), mapCanvas()->mapSettings().destinationCrs() ) );
}

void QgisApp::showStatusMessage( const QString &message )
{
  mStatusBar->showMessage( message );
}

void QgisApp::loadingLayerMessages( const QString &layerName, const QList<QgsReadWriteContext::ReadWriteMessage> &messages )
{
  QVector<QgsReadWriteContext::ReadWriteMessage> shownMessages;
  for ( const QgsReadWriteContext::ReadWriteMessage &message : messages )
  {
    if ( shownMessages.contains( message ) )
      continue;

    visibleMessageBar()->pushMessage( layerName, message.message(), message.categories().join( '\n' ), message.level() );

    shownMessages.append( message );
  }
}

void QgisApp::displayMapToolMessage( const QString &message, Qgis::MessageLevel level )
{
  // remove previous message
  messageBar()->popWidget( mLastMapToolMessage );

  QgsMapTool *tool = mapCanvas()->mapTool();

  if ( tool )
  {
    mLastMapToolMessage = new QgsMessageBarItem( tool->toolName(), message, level );
    messageBar()->pushItem( mLastMapToolMessage );
  }
}

void QgisApp::displayMessage( const QString &title, const QString &message, Qgis::MessageLevel level )
{
  visibleMessageBar()->pushMessage( title, message, level );
}

void QgisApp::removeMapToolMessage()
{
  // remove previous message
  messageBar()->popWidget( mLastMapToolMessage );
}


// Show the maptip using tooltip
void QgisApp::showMapTip()
{
  // Only show maptips if the mouse is still over the map canvas when timer is triggered
  if ( mMapCanvas->underMouse() )
  {
    QPoint myPointerPos = mMapCanvas->mouseLastXY();

    //  Make sure there is an active layer before proceeding
    QgsMapLayer *mypLayer = mMapCanvas->currentLayer();
    if ( mypLayer )
    {
      // only process vector layers
      if ( mypLayer->type() == QgsMapLayerType::VectorLayer )
      {
        // Show the maptip if the maptips button is depressed
        if ( mMapTipsVisible )
        {
          mpMaptip->showMapTip( mypLayer, mLastMapPosition, myPointerPos, mMapCanvas );
        }
      }
    }
  }
}

void QgisApp::projectPropertiesProjections()
{
  // display the project props dialog and switch to the projections tab
  projectProperties( QStringLiteral( "mProjOptsCRS" ) );
}

void QgisApp::projectProperties( const QString &currentPage )
{
  QList< QgsOptionsWidgetFactory * > factories;
  const auto constProjectPropertiesWidgetFactories = mProjectPropertiesWidgetFactories;
  for ( const QPointer< QgsOptionsWidgetFactory > &f : constProjectPropertiesWidgetFactories )
  {
    if ( f )
      factories << f;
  }
  QgsProjectProperties pp( mMapCanvas, this, QgsGuiUtils::ModalDialogFlags, factories );

  qApp->processEvents();

  // Be told if the mouse display precision may have changed by the user
  // changing things in the project properties dialog box
  connect( &pp, &QgsProjectProperties::displayPrecisionChanged, this,
           &QgisApp::updateMouseCoordinatePrecision );

  if ( !currentPage.isEmpty() )
  {
    pp.setCurrentPage( currentPage );
  }
  // Display the modal dialog box.
  pp.exec();

  mMapTools->mapTool< QgsMeasureTool >( QgsAppMapTools::MeasureDistance )->updateSettings();
  mMapTools->mapTool< QgsMeasureTool >( QgsAppMapTools::MeasureArea )->updateSettings();
  mMapTools->mapTool< QgsMapToolMeasureAngle >( QgsAppMapTools::MeasureAngle )->updateSettings();
  mMapTools->mapTool< QgsMapToolMeasureBearing >( QgsAppMapTools::MeasureBearing )->updateSettings();

  // Set the window title.
  setTitleBarText_( *this );
}


QgsClipboard *QgisApp::clipboard()
{
  return mInternalClipboard;
}

void QgisApp::selectionChanged( QgsMapLayer *layer )
{
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
  if ( vlayer )
  {
    const int selectedCount = vlayer->selectedFeatureCount();
    if ( selectedCount == 1 )
    {
      QgsExpressionContext context( QgsExpressionContextUtils::globalProjectLayerScopes( vlayer ) );
      QgsExpression exp = vlayer->displayExpression();
      exp.prepare( &context );

      QgsFeatureRequest request = QgsFeatureRequest().setSubsetOfAttributes( exp.referencedColumns(), vlayer->fields() );
      if ( !exp.needsGeometry() )
        request.setFlags( request.flags() | QgsFeatureRequest::NoGeometry );

      QgsFeature feat;
      QgsFeatureIterator featureIt = vlayer->getSelectedFeatures( request );
      while ( featureIt.nextFeature( feat ) )
      {
        context.setFeature( feat );
        QString featureTitle = exp.evaluate( &context ).toString();
        showStatusMessage( tr( "1 feature selected on layer %1 (%2)." ).arg( vlayer->name(), featureTitle ) );
        break;
      }
    }
    else
    {
      showStatusMessage( tr( "%n feature(s) selected on layer %1.", "number of selected features", selectedCount ).arg( vlayer->name() ) );
    }
  }
  if ( layer == activeLayer() )
  {
    activateDeactivateLayerRelatedActions( layer );
  }

  activateDeactivateMultipleLayersRelatedActions();
}

void QgisApp::legendLayerSelectionChanged()
{
  const QList<QgsLayerTreeLayer *> selectedLayers = mLayerTreeView ? mLayerTreeView->selectedLayerNodes() : QList<QgsLayerTreeLayer *>();

  if ( selectedLayers.empty() && mLayerTreeView )
  {
    // check if a group node alone is selected
    const QList<QgsLayerTreeNode *> selectedNodes = mLayerTreeView->selectedNodes();
    if ( selectedNodes.size() == 1 && QgsLayerTree::isGroup( selectedNodes.at( 0 ) ) )
    {
      QgsLayerTreeGroup *groupNode = QgsLayerTree::toGroup( selectedNodes.at( 0 ) );
      mMapStyleWidget->setEnabled( true );
      if ( mMapStylingDock->isVisible() )
      {
        mMapStyleWidget->setLayerTreeGroup( groupNode );
      }
    }
  }

  mActionDuplicateLayer->setEnabled( !selectedLayers.isEmpty() );
  mActionSetLayerScaleVisibility->setEnabled( !selectedLayers.isEmpty() );
  mActionSetLayerCRS->setEnabled( !selectedLayers.isEmpty() );
  mActionSetProjectCRSFromLayer->setEnabled( selectedLayers.count() == 1 );

  mActionSaveEdits->setEnabled( QgsLayerTreeUtils::layersModified( selectedLayers ) );
  mActionRollbackEdits->setEnabled( QgsLayerTreeUtils::layersModified( selectedLayers ) );
  mActionCancelEdits->setEnabled( QgsLayerTreeUtils::layersEditable( selectedLayers ) );

  mLegendExpressionFilterButton->setEnabled( false );
  mLegendExpressionFilterButton->setVectorLayer( nullptr );
  if ( selectedLayers.size() == 1 )
  {
    QgsLayerTreeLayer *l = selectedLayers.front();
    if ( l->layer() && l->layer()->type() == QgsMapLayerType::VectorLayer )
    {
      mLegendExpressionFilterButton->setEnabled( true );
      bool exprEnabled;
      QString expr = QgsLayerTreeUtils::legendFilterByExpression( *l, &exprEnabled );
      mLegendExpressionFilterButton->setExpressionText( expr );
      mLegendExpressionFilterButton->setVectorLayer( qobject_cast<QgsVectorLayer *>( l->layer() ) );
      mLegendExpressionFilterButton->setChecked( exprEnabled );
    }
  }

  // remove action - check for required layers
  bool removeEnabled = true;
  for ( QgsLayerTreeLayer *nodeLayer : selectedLayers )
  {
    if ( nodeLayer->layer() && !nodeLayer->layer()->flags().testFlag( QgsMapLayer::Removable ) )
    {
      removeEnabled = false;
      break;
    }
  }
  mActionRemoveLayer->setEnabled( removeEnabled );
}

void QgisApp::layerEditStateChanged()
{
  QgsMapLayer *layer = qobject_cast<QgsMapLayer *>( sender() );
  if ( layer && layer == activeLayer() )
  {
    activateDeactivateLayerRelatedActions( layer );
    mSaveRollbackInProgress = false;
  }
}

void QgisApp::updateLabelToolButtons()
{
  bool enableMove = false, enableRotate = false, enablePin = false, enableShowHide = false, enableChange = false;

  QMap<QString, QgsMapLayer *> layers = QgsProject::instance()->mapLayers();
  for ( QMap<QString, QgsMapLayer *>::iterator it = layers.begin(); it != layers.end(); ++it )
  {
    QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( it.value() );
    if ( vlayer && ( vlayer->diagramsEnabled() || vlayer->labelsEnabled() ) )
    {
      enablePin = true;
      enableShowHide = true;
      enableMove = true;
      enableRotate = true;
      enableChange = true;

      break;
    }
  }

  mActionPinLabels->setEnabled( enablePin );
  mActionShowHideLabels->setEnabled( enableShowHide );
  mActionMoveLabel->setEnabled( enableMove );
  mActionRotateLabel->setEnabled( enableRotate );
  mActionChangeLabelProperties->setEnabled( enableChange );
}

bool QgisApp::selectedLayersHaveSelection()
{
  const QList<QgsMapLayer *> layers = mLayerTreeView->selectedLayers();

  // If no selected layers, use active layer
  if ( layers.empty() && activeLayer() )
  {
    if ( QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( activeLayer() ) )
      return layer->selectedFeatureCount() > 0;
  }

  for ( QgsMapLayer *mapLayer : layers )
  {
    QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( mapLayer );

    if ( !layer || !layer->isSpatial() || layer->selectedFeatureCount() == 0 )
      continue;

    return true;
  }

  return false;
}

bool QgisApp::selectedLayersHaveSpatial()
{
  const QList<QgsMapLayer *> layers = mLayerTreeView->selectedLayers();

  // If no selected layers, use active layer
  if ( layers.empty() && activeLayer() )
    return activeLayer()->isSpatial();

  for ( QgsMapLayer *mapLayer : layers )
  {
    if ( !mapLayer || !mapLayer->isSpatial() )
      continue;

    return true;
  }

  return false;
}

void QgisApp::activateDeactivateMultipleLayersRelatedActions()
{
  // these actions are enabled whenever ANY selected layer is spatial
  const bool hasSpatial = selectedLayersHaveSpatial();
  mActionZoomToLayers->setEnabled( hasSpatial );

  // this action is enabled whenever ANY selected layer has a selection
  const bool hasSelection = selectedLayersHaveSelection();
  mActionPanToSelected->setEnabled( hasSelection );
  mActionZoomToSelected->setEnabled( hasSelection );
}

void QgisApp::activateDeactivateLayerRelatedActions( QgsMapLayer *layer )
{
  updateLabelToolButtons();

  mMenuPasteAs->setEnabled( clipboard() && !clipboard()->isEmpty() );
  mActionPasteAsNewVector->setEnabled( clipboard() && !clipboard()->isEmpty() );
  mActionPasteAsNewMemoryVector->setEnabled( clipboard() && !clipboard()->isEmpty() );

  updateLayerModifiedActions();

  QgsAbstractMapToolHandler::Context context;
  for ( QgsAbstractMapToolHandler *handler : std::as_const( mMapToolHandlers ) )
  {
    handler->action()->setEnabled( handler->isCompatibleWithLayer( layer, context ) );
    if ( handler->mapTool() == mMapCanvas->mapTool() )
    {
      if ( !handler->action()->isEnabled() )
      {
        mMapCanvas->unsetMapTool( handler->mapTool() );
        mActionPan->trigger();
      }
      else
      {
        handler->setLayerForTool( layer );
      }
    }
  }

  bool identifyModeIsActiveLayer = QgsSettings().enumValue( QStringLiteral( "/Map/identifyMode" ), QgsMapToolIdentify::ActiveLayer ) == QgsMapToolIdentify::ActiveLayer;

  if ( !layer )
  {
    mMenuSelect->setEnabled( false );
    mActionSelectFeatures->setEnabled( false );
    mActionSelectPolygon->setEnabled( false );
    mActionSelectFreehand->setEnabled( false );
    mActionSelectRadius->setEnabled( false );
    mActionIdentify->setEnabled( true );
    mActionSelectByExpression->setEnabled( false );
    mActionSelectByForm->setEnabled( false );
    mActionLabeling->setEnabled( false );
    mActionOpenTable->setEnabled( false );
    mMenuFilterTable->setEnabled( false );
    mActionOpenTableSelected->setEnabled( false );
    mActionOpenTableVisible->setEnabled( false );
    mActionOpenTableEdited->setEnabled( false );
    mActionSelectAll->setEnabled( false );
    mActionReselect->setEnabled( false );
    mActionInvertSelection->setEnabled( false );
    mActionOpenFieldCalc->setEnabled( false );
    mActionToggleEditing->setEnabled( false );
    mActionToggleEditing->setChecked( false );
    mActionSaveLayerEdits->setEnabled( false );
    mActionSaveLayerDefinition->setEnabled( false );
    mActionLayerSaveAs->setEnabled( false );
    mActionLayerProperties->setEnabled( false );
    mActionLayerSubsetString->setEnabled( false );
    mActionAddToOverview->setEnabled( false );
    mActionFeatureAction->setEnabled( false );
    mActionAddFeature->setEnabled( false );
    mMenuEditGeometry->setEnabled( false );
    mActionMoveFeature->setEnabled( false );
    mActionMoveFeatureCopy->setEnabled( false );
    mActionRotateFeature->setEnabled( false );
    mActionScaleFeature->setEnabled( false );
    mActionOffsetCurve->setEnabled( false );
    mActionVertexTool->setEnabled( false );
    mActionVertexToolActiveLayer->setEnabled( false );
    mActionDeleteSelected->setEnabled( false );
    mActionCutFeatures->setEnabled( false );
    mActionCopyFeatures->setEnabled( false );
    mActionPasteFeatures->setEnabled( false );
    mActionCopyStyle->setEnabled( false );
    mActionPasteStyle->setEnabled( false );
    mActionCopyLayer->setEnabled( false );
    // pasting should be allowed if there is a layer in the clipboard
    mActionPasteLayer->setEnabled( clipboard()->hasFormat( QStringLiteral( QGSCLIPBOARD_MAPLAYER_MIME ) ) );
    mActionReverseLine->setEnabled( false );
    mActionTrimExtendFeature->setEnabled( false );

    if ( mUndoDock && mUndoDock->widget() )
      mUndoDock->widget()->setEnabled( false );
    mActionUndo->setEnabled( false );
    mActionRedo->setEnabled( false );
    mActionSimplifyFeature->setEnabled( false );
    mActionAddRing->setEnabled( false );
    mActionFillRing->setEnabled( false );
    mActionAddPart->setEnabled( false );
    mActionDeleteRing->setEnabled( false );
    mActionDeletePart->setEnabled( false );
    mActionReshapeFeatures->setEnabled( false );
    mActionSplitFeatures->setEnabled( false );
    mActionSplitParts->setEnabled( false );
    mActionMergeFeatures->setEnabled( false );
    mMenuEditAttributes->setEnabled( false );
    mActionMergeFeatureAttributes->setEnabled( false );
    mActionMultiEditAttributes->setEnabled( false );
    mActionRotatePointSymbols->setEnabled( false );
    mActionOffsetPointSymbol->setEnabled( false );

    mActionPinLabels->setEnabled( false );
    mActionShowHideLabels->setEnabled( false );
    mActionMoveLabel->setEnabled( false );
    mActionRotateLabel->setEnabled( false );
    mActionChangeLabelProperties->setEnabled( false );

    mActionDiagramProperties->setEnabled( false );

    mActionLocalHistogramStretch->setEnabled( false );
    mActionFullHistogramStretch->setEnabled( false );
    mActionLocalCumulativeCutStretch->setEnabled( false );
    mActionFullCumulativeCutStretch->setEnabled( false );
    mActionIncreaseBrightness->setEnabled( false );
    mActionDecreaseBrightness->setEnabled( false );
    mActionIncreaseContrast->setEnabled( false );
    mActionDecreaseContrast->setEnabled( false );
    mActionIncreaseGamma->setEnabled( false );
    mActionDecreaseGamma->setEnabled( false );
    mActionPanToSelected->setEnabled( false );
    mActionZoomActualSize->setEnabled( false );
    mActionZoomToSelected->setEnabled( false );
    mActionZoomToLayers->setEnabled( false );
    mActionZoomToLayer->setEnabled( false );

    //enableMeshEditingTools( false ); // SG add
    mDigitizingTechniqueManager->enableDigitizingTechniqueActions( false );

    return;
  }

  mMenuSelect->setEnabled( true );

  mActionLayerProperties->setEnabled( QgsProject::instance()->layerIsEmbedded( layer->id() ).isEmpty() );
  mActionAddToOverview->setEnabled( true );
  mActionPanToSelected->setEnabled( true );
  mActionZoomToSelected->setEnabled( true );
  mActionZoomToLayers->setEnabled( true );
  mActionZoomToLayer->setEnabled( true );

  mActionCopyStyle->setEnabled( true );
  mActionPasteStyle->setEnabled( clipboard()->hasFormat( QStringLiteral( QGSCLIPBOARD_STYLE_MIME ) ) );
  mActionCopyLayer->setEnabled( true );

  // Vector layers
  switch ( layer->type() )
  {
    case QgsMapLayerType::VectorLayer:
    {
      QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( layer );
      QgsVectorDataProvider *dprovider = vlayer->dataProvider();
      QString addFeatureText;

      bool isEditable = vlayer->isEditable();
      bool layerHasSelection = vlayer->selectedFeatureCount() > 0;
      bool layerHasActions = !vlayer->actions()->actions( QStringLiteral( "Canvas" ) ).isEmpty() || !QgsGui::mapLayerActionRegistry()->mapLayerActions( vlayer ).isEmpty();
      bool isSpatial = vlayer->isSpatial();

      mActionLocalHistogramStretch->setEnabled( false );
      mActionFullHistogramStretch->setEnabled( false );
      mActionLocalCumulativeCutStretch->setEnabled( false );
      mActionFullCumulativeCutStretch->setEnabled( false );
      mActionIncreaseBrightness->setEnabled( false );
      mActionDecreaseBrightness->setEnabled( false );
      mActionIncreaseContrast->setEnabled( false );
      mActionDecreaseContrast->setEnabled( false );
      mActionIncreaseGamma->setEnabled( false );
      mActionDecreaseGamma->setEnabled( false );
      mActionZoomActualSize->setEnabled( false );
      mActionZoomToLayer->setEnabled( isSpatial );
      mActionLabeling->setEnabled( isSpatial );
      mActionDiagramProperties->setEnabled( isSpatial );
      mActionReverseLine->setEnabled( false );
      mActionTrimExtendFeature->setEnabled( false );

      //enableMeshEditingTools( false ); // SG add

      mActionSelectFeatures->setEnabled( isSpatial );
      mActionSelectPolygon->setEnabled( isSpatial );
      mActionSelectFreehand->setEnabled( isSpatial );
      mActionSelectRadius->setEnabled( isSpatial );
      mActionIdentify->setEnabled( isSpatial || !identifyModeIsActiveLayer );
      mActionSelectByExpression->setEnabled( true );
      mActionSelectByForm->setEnabled( true );
      mActionOpenTable->setEnabled( true );
      mMenuFilterTable->setEnabled( true );
      mActionOpenTableSelected->setEnabled( true );
      mActionOpenTableVisible->setEnabled( true );
      mActionOpenTableEdited->setEnabled( true );
      mActionSelectAll->setEnabled( true );
      mActionReselect->setEnabled( true );
      mActionInvertSelection->setEnabled( true );
      mActionSaveLayerDefinition->setEnabled( true );
      mActionLayerSaveAs->setEnabled( true );
      mActionCopyFeatures->setEnabled( layerHasSelection );
      mActionFeatureAction->setEnabled( layerHasActions );

      if ( !isEditable && mMapCanvas && mMapCanvas->mapTool()
           && ( mMapCanvas->mapTool()->flags() & QgsMapTool::EditTool ) && !mSaveRollbackInProgress )
      {
        mMapCanvas->setMapTool( mNonEditMapTool );
      }

      if ( dprovider )
      {
        bool canChangeAttributes = dprovider->capabilities() & QgsVectorDataProvider::ChangeAttributeValues;
        bool canDeleteFeatures = dprovider->capabilities() & QgsVectorDataProvider::DeleteFeatures;
        bool canAddFeatures = dprovider->capabilities() & QgsVectorDataProvider::AddFeatures;
        bool canChangeGeometry = isSpatial && dprovider->capabilities() & QgsVectorDataProvider::ChangeGeometries;
        bool canSupportEditing = vlayer->supportsEditing();

        mActionLayerSubsetString->setEnabled( !isEditable && dprovider->supportsSubsetString() );

        mActionToggleEditing->setEnabled( canSupportEditing );
        mActionToggleEditing->setChecked( canSupportEditing && isEditable );
        mActionSaveLayerEdits->setEnabled( canSupportEditing && isEditable && vlayer->isModified() );
        mUndoDock->widget()->setEnabled( canSupportEditing && isEditable );
        mActionUndo->setEnabled( canSupportEditing );
        mActionRedo->setEnabled( canSupportEditing );
        mMenuEditGeometry->setEnabled( canSupportEditing && isEditable );

        //start editing/stop editing
        if ( canSupportEditing )
        {
          updateUndoActions();
        }

        mActionPasteFeatures->setEnabled( isEditable && canAddFeatures && !clipboard()->isEmpty() );

        mActionAddFeature->setEnabled( isEditable && canAddFeatures );

        //does provider allow deleting of features?
        mActionDeleteSelected->setEnabled( isEditable && canDeleteFeatures && layerHasSelection );
        mActionCutFeatures->setEnabled( isEditable && canDeleteFeatures && layerHasSelection );

        //merge tool needs editable layer and provider with the capability of adding and deleting features
        if ( isEditable && canChangeAttributes )
        {
          mActionMergeFeatures->setEnabled( layerHasSelection && canDeleteFeatures && canAddFeatures );
          mMenuEditAttributes->setEnabled( layerHasSelection );
          mActionMergeFeatureAttributes->setEnabled( layerHasSelection );
          mActionMultiEditAttributes->setEnabled( layerHasSelection );
        }
        else
        {
          mActionMergeFeatures->setEnabled( false );
          mMenuEditAttributes->setEnabled( false );
          mActionMergeFeatureAttributes->setEnabled( false );
          mActionMultiEditAttributes->setEnabled( false );
        }

        bool isMultiPart = QgsWkbTypes::isMultiType( vlayer->wkbType() ) || !dprovider->doesStrictFeatureTypeCheck();

        // moving enabled if geometry changes are supported
        mActionAddPart->setEnabled( isEditable && canChangeGeometry );
        mActionDeletePart->setEnabled( isEditable && canChangeGeometry );
        mActionMoveFeature->setEnabled( isEditable && canChangeGeometry );
        mActionMoveFeatureCopy->setEnabled( isEditable && canChangeGeometry );
        mActionRotateFeature->setEnabled( isEditable && canChangeGeometry );
        mActionScaleFeature->setEnabled( isEditable && canChangeGeometry );
        mActionVertexTool->setEnabled( isEditable && canChangeGeometry );
        mActionVertexToolActiveLayer->setEnabled( isEditable && canChangeGeometry );

        mDigitizingTechniqueManager->enableDigitizingTechniqueActions( isEditable && canChangeGeometry );

        if ( vlayer->geometryType() == QgsWkbTypes::PointGeometry )
        {
          mActionAddFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCapturePoint.svg" ) ) );
          addFeatureText = tr( "Add Point Feature" );
          mActionMoveFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveFeaturePoint.svg" ) ) );
          mActionMoveFeatureCopy->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveFeatureCopyPoint.svg" ) ) );

          mActionAddRing->setEnabled( false );
          mActionFillRing->setEnabled( false );
          mActionReshapeFeatures->setEnabled( false );
          mActionSplitFeatures->setEnabled( false );
          mActionSplitParts->setEnabled( false );
          mActionSimplifyFeature->setEnabled( false );
          mActionDeleteRing->setEnabled( false );
          mActionRotatePointSymbols->setEnabled( false );
          mActionOffsetPointSymbol->setEnabled( false );
          mActionOffsetCurve->setEnabled( false );

          if ( isEditable && canChangeAttributes )
          {
            if ( QgsMapToolRotatePointSymbols::layerIsRotatable( vlayer ) )
            {
              mActionRotatePointSymbols->setEnabled( true );
            }
            if ( QgsMapToolOffsetPointSymbol::layerIsOffsetable( vlayer ) )
            {
              mActionOffsetPointSymbol->setEnabled( true );
            }
          }
        }
        else if ( vlayer->geometryType() == QgsWkbTypes::LineGeometry )
        {
          mActionAddFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCaptureLine.svg" ) ) );
          addFeatureText = tr( "Add Line Feature" );
          mActionMoveFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveFeatureLine.svg" ) ) );
          mActionMoveFeatureCopy->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveFeatureCopyLine.svg" ) ) );

          mActionReshapeFeatures->setEnabled( isEditable && canChangeGeometry );
          mActionSplitFeatures->setEnabled( isEditable && canAddFeatures );
          mActionSplitParts->setEnabled( isEditable && canChangeGeometry && isMultiPart );
          mActionSimplifyFeature->setEnabled( isEditable && canChangeGeometry );
          mActionOffsetCurve->setEnabled( isEditable && canAddFeatures && canChangeAttributes );
          mActionReverseLine->setEnabled( isEditable && canChangeGeometry );
          mActionTrimExtendFeature->setEnabled( isEditable && canChangeGeometry );

          mActionAddRing->setEnabled( false );
          mActionFillRing->setEnabled( false );
          mActionDeleteRing->setEnabled( false );
        }
        else if ( vlayer->geometryType() == QgsWkbTypes::PolygonGeometry )
        {
          mActionAddFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionCapturePolygon.svg" ) ) );
          addFeatureText = tr( "Add Polygon Feature" );
          mActionMoveFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveFeature.svg" ) ) );
          mActionMoveFeatureCopy->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionMoveFeatureCopy.svg" ) ) );

          mActionAddRing->setEnabled( isEditable && canChangeGeometry );
          mActionFillRing->setEnabled( isEditable && canChangeGeometry );
          mActionReshapeFeatures->setEnabled( isEditable && canChangeGeometry );
          mActionSplitFeatures->setEnabled( isEditable && canAddFeatures );
          mActionSplitParts->setEnabled( isEditable && canChangeGeometry && isMultiPart );
          mActionSimplifyFeature->setEnabled( isEditable && canChangeGeometry );
          mActionDeleteRing->setEnabled( isEditable && canChangeGeometry );
          mActionOffsetCurve->setEnabled( isEditable && canAddFeatures && canChangeAttributes );
          mActionTrimExtendFeature->setEnabled( isEditable && canChangeGeometry );
        }
        else if ( vlayer->geometryType() == QgsWkbTypes::NullGeometry )
        {
          mActionAddFeature->setIcon( QgsApplication::getThemeIcon( QStringLiteral( "/mActionNewTableRow.svg" ) ) );
          addFeatureText = tr( "Add Record" );
          mActionAddRing->setEnabled( false );
          mActionFillRing->setEnabled( false );
          mActionReshapeFeatures->setEnabled( false );
          mActionSplitFeatures->setEnabled( false );
          mActionSplitParts->setEnabled( false );
          mActionSimplifyFeature->setEnabled( false );
          mActionDeleteRing->setEnabled( false );
          mActionOffsetCurve->setEnabled( false );
        }

        mActionOpenFieldCalc->setEnabled( true );
        mActionAddFeature->setText( addFeatureText );
        mActionAddFeature->setToolTip( addFeatureText );
        QgsGui::shortcutsManager()->unregisterAction( mActionAddFeature );
        if ( !mActionAddFeature->text().isEmpty() ) // The text will be empty on unknown geometry type -> in this case do not create a shortcut
          QgsGui::shortcutsManager()->registerAction( mActionAddFeature, mActionAddFeature->shortcut().toString() );
      }
      else
      {
        mUndoDock->widget()->setEnabled( false );
        mActionUndo->setEnabled( false );
        mActionRedo->setEnabled( false );
        mActionLayerSubsetString->setEnabled( false );
      }
      break;
    }

    case QgsMapLayerType::RasterLayer:
    {
      const QgsRasterLayer *rlayer = qobject_cast<const QgsRasterLayer *>( layer );
      const QgsRasterDataProvider *dprovider = rlayer->dataProvider();

      if ( dprovider
           && dprovider->dataType( 1 ) != Qgis::DataType::ARGB32
           && dprovider->dataType( 1 ) != Qgis::DataType::ARGB32_Premultiplied )
      {
        if ( dprovider->capabilities() & QgsRasterDataProvider::Size )
        {
          mActionFullHistogramStretch->setEnabled( true );
        }
        else
        {
          // it would hang up reading the data for WMS for example
          mActionFullHistogramStretch->setEnabled( false );
        }
        mActionLocalHistogramStretch->setEnabled( true );
      }
      else
      {
        mActionLocalHistogramStretch->setEnabled( false );
        mActionFullHistogramStretch->setEnabled( false );
      }

      mActionLocalCumulativeCutStretch->setEnabled( true );
      mActionFullCumulativeCutStretch->setEnabled( true );
      mActionIncreaseBrightness->setEnabled( true );
      mActionDecreaseBrightness->setEnabled( true );
      mActionIncreaseContrast->setEnabled( true );
      mActionDecreaseContrast->setEnabled( true );
      mActionIncreaseGamma->setEnabled( true );
      mActionDecreaseGamma->setEnabled( true );

      mActionLayerSubsetString->setEnabled( false );
      mActionFeatureAction->setEnabled( false );
      mActionSelectFeatures->setEnabled( false );
      mActionSelectPolygon->setEnabled( false );
      mActionSelectFreehand->setEnabled( false );
      mActionSelectRadius->setEnabled( false );
      mActionZoomActualSize->setEnabled( true );
      mActionZoomToLayer->setEnabled( true );
      mActionOpenTable->setEnabled( false );
      mMenuFilterTable->setEnabled( false );
      mActionOpenTableSelected->setEnabled( false );
      mActionOpenTableVisible->setEnabled( false );
      mActionOpenTableEdited->setEnabled( false );
      mActionSelectAll->setEnabled( false );
      mActionReselect->setEnabled( false );
      mActionInvertSelection->setEnabled( false );
      mActionSelectByExpression->setEnabled( false );
      mActionSelectByForm->setEnabled( false );
      mActionOpenFieldCalc->setEnabled( false );
      mActionToggleEditing->setEnabled( false );
      mActionToggleEditing->setChecked( false );
      mActionSaveLayerEdits->setEnabled( false );
      mUndoDock->widget()->setEnabled( false );
      mActionUndo->setEnabled( false );
      mActionRedo->setEnabled( false );
      mActionSaveLayerDefinition->setEnabled( true );
      mActionLayerSaveAs->setEnabled( true );
      mActionAddFeature->setEnabled( false );
      mMenuEditAttributes->setEnabled( false );
      mMenuEditGeometry->setEnabled( false );
      mActionReverseLine->setEnabled( false );
      mActionTrimExtendFeature->setEnabled( false );
      mActionDeleteSelected->setEnabled( false );
      mActionAddRing->setEnabled( false );
      mActionFillRing->setEnabled( false );
      mActionAddPart->setEnabled( false );
      mActionVertexTool->setEnabled( false );
      mActionVertexToolActiveLayer->setEnabled( false );
      mActionMoveFeature->setEnabled( false );
      mActionMoveFeatureCopy->setEnabled( false );
      mActionRotateFeature->setEnabled( false );
      mActionScaleFeature->setEnabled( false );
      mActionOffsetCurve->setEnabled( false );
      mActionCopyFeatures->setEnabled( false );
      mActionCutFeatures->setEnabled( false );
      mActionPasteFeatures->setEnabled( false );
      mActionRotatePointSymbols->setEnabled( false );
      mActionOffsetPointSymbol->setEnabled( false );
      mActionDeletePart->setEnabled( false );
      mActionDeleteRing->setEnabled( false );
      mActionSimplifyFeature->setEnabled( false );
      mActionReshapeFeatures->setEnabled( false );
      mActionSplitFeatures->setEnabled( false );
      mActionSplitParts->setEnabled( false );
      mActionLabeling->setEnabled( false );
      mActionDiagramProperties->setEnabled( false );

      //enableMeshEditingTools( false ); // SG add
      mDigitizingTechniqueManager->enableDigitizingTechniqueActions( false );

      //NOTE: This check does not really add any protection, as it is called on load not on layer select/activate
      //If you load a layer with a provider and idenitfy ability then load another without, the tool would be disabled for both

      //Enable the Identify tool ( GDAL datasets draw without a provider )
      //but turn off if data provider exists and has no Identify capabilities
      mActionIdentify->setEnabled( true );

      if ( identifyModeIsActiveLayer )
      {
        if ( dprovider )
        {
          // does provider allow the identify map tool?
          if ( dprovider->capabilities() & QgsRasterDataProvider::Identify )
          {
            mActionIdentify->setEnabled( true );
          }
          else
          {
            mActionIdentify->setEnabled( false );
          }
        }
      }
      break;
    }

    case QgsMapLayerType::MeshLayer:
    {
      QgsMeshLayer *mlayer = qobject_cast<QgsMeshLayer *>( layer );

      mActionLocalHistogramStretch->setEnabled( false );
      mActionFullHistogramStretch->setEnabled( false );
      mActionLocalCumulativeCutStretch->setEnabled( false );
      mActionFullCumulativeCutStretch->setEnabled( false );
      mActionIncreaseBrightness->setEnabled( false );
      mActionDecreaseBrightness->setEnabled( false );
      mActionIncreaseContrast->setEnabled( false );
      mActionDecreaseContrast->setEnabled( false );
      mActionIncreaseGamma->setEnabled( false );
      mActionDecreaseGamma->setEnabled( false );
      mActionLayerSubsetString->setEnabled( false );
      mActionFeatureAction->setEnabled( false );
      mActionSelectFeatures->setEnabled( false );
      mActionSelectPolygon->setEnabled( false );
      mActionSelectFreehand->setEnabled( false );
      mActionSelectRadius->setEnabled( false );
      mActionZoomActualSize->setEnabled( false );
      mActionZoomToLayer->setEnabled( true );
      mActionOpenTable->setEnabled( false );
      mMenuFilterTable->setEnabled( false );
      mActionOpenTableSelected->setEnabled( false );
      mActionOpenTableVisible->setEnabled( false );
      mActionOpenTableEdited->setEnabled( false );
      mActionSelectAll->setEnabled( false );
      mActionReselect->setEnabled( false );
      mActionInvertSelection->setEnabled( false );
      mActionSelectByExpression->setEnabled( false );
      mActionSelectByForm->setEnabled( false );
      mActionOpenFieldCalc->setEnabled( false );
      mActionSaveLayerEdits->setEnabled( false );
      mActionSaveLayerDefinition->setEnabled( true );
      mActionLayerSaveAs->setEnabled( false );
      mActionAddFeature->setEnabled( false );
      mActionDeleteSelected->setEnabled( false );
      mActionAddRing->setEnabled( false );
      mActionFillRing->setEnabled( false );
      mActionAddPart->setEnabled( false );
      mActionVertexTool->setEnabled( false );
      mActionVertexToolActiveLayer->setEnabled( false );
      mActionMoveFeature->setEnabled( false );
      mActionMoveFeatureCopy->setEnabled( false );
      mActionRotateFeature->setEnabled( false );
      mActionScaleFeature->setEnabled( false );
      mActionOffsetCurve->setEnabled( false );
      mActionCopyFeatures->setEnabled( false );
      mActionCutFeatures->setEnabled( false );
      mActionPasteFeatures->setEnabled( false );
      mActionRotatePointSymbols->setEnabled( false );
      mActionOffsetPointSymbol->setEnabled( false );
      mActionDeletePart->setEnabled( false );
      mActionDeleteRing->setEnabled( false );
      mActionSimplifyFeature->setEnabled( false );
      mActionReshapeFeatures->setEnabled( false );
      mActionSplitFeatures->setEnabled( false );
      mActionSplitParts->setEnabled( false );
      mActionLabeling->setEnabled( false );
      mActionDiagramProperties->setEnabled( false );
      mActionIdentify->setEnabled( true );
      mDigitizingTechniqueManager->enableDigitizingTechniqueActions( false );

      bool canSupportEditing = mlayer->supportsEditing();
      bool isEditable = mlayer->isEditable();
      mActionToggleEditing->setEnabled( canSupportEditing );
      mActionToggleEditing->setChecked( canSupportEditing && isEditable );
      mActionSaveLayerEdits->setEnabled( canSupportEditing && isEditable && mlayer->isModified() );
      //enableMeshEditingTools( isEditable ); // SG add
      mUndoDock->widget()->setEnabled( canSupportEditing && isEditable );
      mActionUndo->setEnabled( canSupportEditing && isEditable );
      mActionRedo->setEnabled( canSupportEditing && isEditable );
      updateUndoActions();
    }

    break;

    case QgsMapLayerType::VectorTileLayer:
      mActionLocalHistogramStretch->setEnabled( false );
      mActionFullHistogramStretch->setEnabled( false );
      mActionLocalCumulativeCutStretch->setEnabled( false );
      mActionFullCumulativeCutStretch->setEnabled( false );
      mActionIncreaseBrightness->setEnabled( false );
      mActionDecreaseBrightness->setEnabled( false );
      mActionIncreaseContrast->setEnabled( false );
      mActionDecreaseContrast->setEnabled( false );
      mActionIncreaseGamma->setEnabled( false );
      mActionDecreaseGamma->setEnabled( false );
      mActionLayerSubsetString->setEnabled( false );
      mActionFeatureAction->setEnabled( false );
      mActionSelectFeatures->setEnabled( false );
      mActionSelectPolygon->setEnabled( false );
      mActionSelectFreehand->setEnabled( false );
      mActionSelectRadius->setEnabled( false );
      mActionZoomActualSize->setEnabled( false );
      mActionZoomToLayer->setEnabled( true );
      mActionOpenTable->setEnabled( false );
      mMenuFilterTable->setEnabled( false );
      mActionOpenTableSelected->setEnabled( false );
      mActionOpenTableVisible->setEnabled( false );
      mActionOpenTableEdited->setEnabled( false );
      mActionSelectAll->setEnabled( false );
      mActionReselect->setEnabled( false );
      mActionInvertSelection->setEnabled( false );
      mActionSelectByExpression->setEnabled( false );
      mActionSelectByForm->setEnabled( false );
      mActionOpenFieldCalc->setEnabled( false );
      mActionToggleEditing->setEnabled( false );
      mActionToggleEditing->setChecked( false );
      mActionSaveLayerEdits->setEnabled( false );
      mUndoDock->widget()->setEnabled( false );
      mActionUndo->setEnabled( false );
      mActionRedo->setEnabled( false );
      mActionSaveLayerDefinition->setEnabled( true );
      mActionLayerSaveAs->setEnabled( false );
      mActionAddFeature->setEnabled( false );
      mActionDeleteSelected->setEnabled( false );
      mActionAddRing->setEnabled( false );
      mActionFillRing->setEnabled( false );
      mActionAddPart->setEnabled( false );
      mActionVertexTool->setEnabled( false );
      mActionVertexToolActiveLayer->setEnabled( false );
      mActionMoveFeature->setEnabled( false );
      mActionMoveFeatureCopy->setEnabled( false );
      mActionRotateFeature->setEnabled( false );
      mActionScaleFeature->setEnabled( false );
      mActionOffsetCurve->setEnabled( false );
      mActionCopyFeatures->setEnabled( false );
      mActionCutFeatures->setEnabled( false );
      mActionPasteFeatures->setEnabled( false );
      mActionRotatePointSymbols->setEnabled( false );
      mActionOffsetPointSymbol->setEnabled( false );
      mActionDeletePart->setEnabled( false );
      mActionDeleteRing->setEnabled( false );
      mActionSimplifyFeature->setEnabled( false );
      mActionReshapeFeatures->setEnabled( false );
      mActionSplitFeatures->setEnabled( false );
      mActionSplitParts->setEnabled( false );
      mActionLabeling->setEnabled( false );
      mActionDiagramProperties->setEnabled( false );
      mActionIdentify->setEnabled( true );
      mDigitizingTechniqueManager->enableDigitizingTechniqueActions( false );
      //enableMeshEditingTools( false ); // SG add
      break;

    case QgsMapLayerType::PointCloudLayer:
    {
      const QgsDataProvider *dprovider = layer->dataProvider();
      mActionLocalHistogramStretch->setEnabled( false );
      mActionFullHistogramStretch->setEnabled( false );
      mActionLocalCumulativeCutStretch->setEnabled( false );
      mActionFullCumulativeCutStretch->setEnabled( false );
      mActionIncreaseBrightness->setEnabled( false );
      mActionDecreaseBrightness->setEnabled( false );
      mActionIncreaseContrast->setEnabled( false );
      mActionDecreaseContrast->setEnabled( false );
      mActionIncreaseGamma->setEnabled( false );
      mActionDecreaseGamma->setEnabled( false );
      mActionLayerSubsetString->setEnabled( dprovider && dprovider->supportsSubsetString() );
      mActionFeatureAction->setEnabled( false );
      mActionSelectFeatures->setEnabled( false );
      mActionSelectPolygon->setEnabled( false );
      mActionSelectFreehand->setEnabled( false );
      mActionSelectRadius->setEnabled( false );
      mActionZoomActualSize->setEnabled( false );
      mActionZoomToLayer->setEnabled( true );
      mActionOpenTable->setEnabled( false );
      mMenuFilterTable->setEnabled( false );
      mActionOpenTableSelected->setEnabled( false );
      mActionOpenTableVisible->setEnabled( false );
      mActionOpenTableEdited->setEnabled( false );
      mActionSelectAll->setEnabled( false );
      mActionReselect->setEnabled( false );
      mActionInvertSelection->setEnabled( false );
      mActionSelectByExpression->setEnabled( false );
      mActionSelectByForm->setEnabled( false );
      mActionOpenFieldCalc->setEnabled( false );
      mActionToggleEditing->setEnabled( false );
      mActionToggleEditing->setChecked( false );
      mActionSaveLayerEdits->setEnabled( false );
      mUndoDock->widget()->setEnabled( false );
      mActionUndo->setEnabled( false );
      mActionRedo->setEnabled( false );
      mActionSaveLayerDefinition->setEnabled( true );
      mActionLayerSaveAs->setEnabled( false );
      mActionAddFeature->setEnabled( false );
      mActionDeleteSelected->setEnabled( false );
      mActionAddRing->setEnabled( false );
      mActionFillRing->setEnabled( false );
      mActionAddPart->setEnabled( false );
      mActionVertexTool->setEnabled( false );
      mActionVertexToolActiveLayer->setEnabled( false );
      mActionMoveFeature->setEnabled( false );
      mActionMoveFeatureCopy->setEnabled( false );
      mActionRotateFeature->setEnabled( false );
      mActionScaleFeature->setEnabled( false );
      mActionOffsetCurve->setEnabled( false );
      mActionCopyFeatures->setEnabled( false );
      mActionCutFeatures->setEnabled( false );
      mActionPasteFeatures->setEnabled( false );
      mActionRotatePointSymbols->setEnabled( false );
      mActionOffsetPointSymbol->setEnabled( false );
      mActionDeletePart->setEnabled( false );
      mActionDeleteRing->setEnabled( false );
      mActionSimplifyFeature->setEnabled( false );
      mActionReshapeFeatures->setEnabled( false );
      mActionSplitFeatures->setEnabled( false );
      mActionSplitParts->setEnabled( false );
      mActionLabeling->setEnabled( false );
      mActionDiagramProperties->setEnabled( false );
      mActionIdentify->setEnabled( true );
      mDigitizingTechniqueManager->enableDigitizingTechniqueActions( false );
      //enableMeshEditingTools( false ); // SG add
      break;
    }
    case QgsMapLayerType::PluginLayer:
    case QgsMapLayerType::GroupLayer:
      break;

    case QgsMapLayerType::AnnotationLayer:
    {
      mActionLocalHistogramStretch->setEnabled( false );
      mActionFullHistogramStretch->setEnabled( false );
      mActionLocalCumulativeCutStretch->setEnabled( false );
      mActionFullCumulativeCutStretch->setEnabled( false );
      mActionIncreaseBrightness->setEnabled( false );
      mActionDecreaseBrightness->setEnabled( false );
      mActionIncreaseContrast->setEnabled( false );
      mActionDecreaseContrast->setEnabled( false );
      mActionIncreaseGamma->setEnabled( false );
      mActionDecreaseGamma->setEnabled( false );
      mActionLayerSubsetString->setEnabled( false );
      mActionFeatureAction->setEnabled( false );
      mActionSelectFeatures->setEnabled( false );
      mActionSelectPolygon->setEnabled( false );
      mActionSelectFreehand->setEnabled( false );
      mActionSelectRadius->setEnabled( false );
      mActionZoomActualSize->setEnabled( false );
      mActionZoomToLayer->setEnabled( true );
      mActionOpenTable->setEnabled( false );
      mMenuFilterTable->setEnabled( false );
      mActionOpenTableSelected->setEnabled( false );
      mActionOpenTableVisible->setEnabled( false );
      mActionOpenTableEdited->setEnabled( false );
      mActionSelectAll->setEnabled( false );
      mActionReselect->setEnabled( false );
      mActionInvertSelection->setEnabled( false );
      mActionSelectByExpression->setEnabled( false );
      mActionSelectByForm->setEnabled( false );
      mActionOpenFieldCalc->setEnabled( false );
      mActionSaveLayerEdits->setEnabled( false );
      mUndoDock->widget()->setEnabled( false );
      mActionSaveLayerDefinition->setEnabled( false );
      mActionLayerSaveAs->setEnabled( false );
      mActionAddFeature->setEnabled( false );
      mActionDeleteSelected->setEnabled( false );
      mActionAddRing->setEnabled( false );
      mActionFillRing->setEnabled( false );
      mActionAddPart->setEnabled( false );
      mActionVertexTool->setEnabled( false );
      mActionVertexToolActiveLayer->setEnabled( false );
      mActionMoveFeature->setEnabled( false );
      mActionMoveFeatureCopy->setEnabled( false );
      mActionRotateFeature->setEnabled( false );
      mActionScaleFeature->setEnabled( false );
      mActionOffsetCurve->setEnabled( false );
      mActionCopyFeatures->setEnabled( false );
      mActionCutFeatures->setEnabled( false );
      mActionPasteFeatures->setEnabled( false );
      mActionRotatePointSymbols->setEnabled( false );
      mActionOffsetPointSymbol->setEnabled( false );
      mActionDeletePart->setEnabled( false );
      mActionDeleteRing->setEnabled( false );
      mActionSimplifyFeature->setEnabled( false );
      mActionReshapeFeatures->setEnabled( false );
      mActionSplitFeatures->setEnabled( false );
      mActionSplitParts->setEnabled( false );
      mActionLabeling->setEnabled( false );
      mActionDiagramProperties->setEnabled( false );
      mActionIdentify->setEnabled( true );
      mDigitizingTechniqueManager->enableDigitizingTechniqueActions( true );
      mActionToggleEditing->setEnabled( false );
      mActionToggleEditing->setChecked( true ); // always editable
      mActionUndo->setEnabled( false );
      mActionRedo->setEnabled( false );
      updateUndoActions();
      break;
    }
  }

  refreshFeatureActions();
}

void QgisApp::refreshActionFeatureAction()
{
  mActionFeatureAction->setEnabled( false );
  QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( activeLayer() );
  if ( !vlayer )
    return;

  bool layerHasActions = !vlayer->actions()->actions( QStringLiteral( "Canvas" ) ).isEmpty() || !QgsGui::mapLayerActionRegistry()->mapLayerActions( vlayer ).isEmpty();
  mActionFeatureAction->setEnabled( layerHasActions );
}

void QgisApp::renameView()
{
  QgsMapCanvasDockWidget *view = qobject_cast< QgsMapCanvasDockWidget * >( sender() );
  if ( !view )
    return;

  // calculate existing names
  QStringList names;
  const auto canvases = mapCanvases();
  for ( QgsMapCanvas *canvas : canvases )
  {
    if ( canvas == view->mapCanvas() )
      continue;

    names << canvas->objectName();
  }

  QString currentName = view->mapCanvas()->objectName();

  QgsNewNameDialog renameDlg( currentName, currentName, QStringList(), names, Qt::CaseSensitive, this );
  renameDlg.setWindowTitle( tr( "Map Views" ) );
  //renameDlg.setHintString( tr( "Name of the new view" ) );
  renameDlg.setOverwriteEnabled( false );
  renameDlg.setConflictingNameWarning( tr( "A view with this name already exists" ) );
  renameDlg.buttonBox()->addButton( QDialogButtonBox::Help );
  connect( renameDlg.buttonBox(), &QDialogButtonBox::helpRequested, this, [ = ]
  {
    QgsHelp::openHelp( QStringLiteral( "introduction/qgis_gui.html#map-view" ) );
  } );

  if ( renameDlg.exec() || renameDlg.name().isEmpty() )
  {
    QString newName = renameDlg.name();
    view->setWindowTitle( newName );
    view->mapCanvas()->setObjectName( newName );
  }
}

QgsRasterLayer *QgisApp::addRasterLayer( QString const &uri, QString const &baseName, QString const &providerKey )
{
  return addLayerPrivate< QgsRasterLayer >( QgsMapLayerType::RasterLayer, uri, baseName, !providerKey.isEmpty() ? providerKey : QLatin1String( "gdal" ), true );
}

bool QgisApp::addRasterLayers( QStringList const &files, bool guiWarning )
{
  if ( files.empty() )
  {
    return false;
  }

  QgsCanvasRefreshBlocker refreshBlocker;

  // this is messy since some files in the list may be rasters and others may
  // be ogr layers. We'll set returnValue to false if one or more layers fail
  // to load.
  bool returnValue = true;
  for ( const QString &src : files )
  {
    QString errMsg;
    bool ok = false;

    // if needed prompt for zipitem layers
    QString vsiPrefix = QgsZipItem::vsiPrefix( src );
    if ( ( !src.startsWith( QLatin1String( "/vsi" ), Qt::CaseInsensitive ) || src.endsWith( QLatin1String( ".zip" ) ) || src.endsWith( QLatin1String( ".tar" ) ) ) &&
         ( vsiPrefix == QLatin1String( "/vsizip/" ) || vsiPrefix == QLatin1String( "/vsitar/" ) ) )
    {
      if ( askUserForZipItemLayers( src, { QgsMapLayerType::RasterLayer } ) )
        continue;
    }

    const bool isVsiCurl { src.startsWith( QLatin1String( "/vsicurl" ), Qt::CaseInsensitive ) };
    const bool isRemoteUrl { src.startsWith( QLatin1String( "http" ) ) || src == QLatin1String( "ftp" ) };

    std::unique_ptr< QgsTemporaryCursorOverride > cursorOverride;
    if ( isVsiCurl || isRemoteUrl )
    {
      cursorOverride = std::make_unique< QgsTemporaryCursorOverride >( Qt::WaitCursor );
      visibleMessageBar()->pushInfo( tr( "Remote layer" ), tr( "loading %1, please wait …" ).arg( src ) );
      qApp->processEvents();
    }

    if ( QgsRasterLayer::isValidRasterFileName( src, errMsg ) )
    {
      QFileInfo myFileInfo( src );

      // set the layer name to the file base name unless provided explicitly
      QString layerName;
      const QVariantMap uriDetails = QgsProviderRegistry::instance()->decodeUri( QStringLiteral( "gdal" ), src );
      if ( !uriDetails[ QStringLiteral( "layerName" ) ].toString().isEmpty() )
      {
        layerName = uriDetails[ QStringLiteral( "layerName" ) ].toString();
      }
      else
      {
        layerName = QgsProviderUtils::suggestLayerNameFromFilePath( src );
      }

      // try to create the layer
      cursorOverride.reset();
      QgsRasterLayer *layer = addLayerPrivate< QgsRasterLayer >( QgsMapLayerType::RasterLayer, src, layerName, QStringLiteral( "gdal" ), guiWarning );

      if ( layer && layer->isValid() )
      {
        //only allow one copy of a ai grid file to be loaded at a
        //time to prevent the user selecting all adfs in 1 dir which
        //actually represent 1 coverage,

        if ( myFileInfo.fileName().endsWith( QLatin1String( ".adf" ), Qt::CaseInsensitive ) )
        {
          break;
        }
      }
      // if layer is invalid addLayerPrivate() will show the error

    } // valid raster filename
    else
    {
      ok = false;

      // Issue message box warning unless we are loading from cmd line since
      // non-rasters are passed to this function first and then successfully
      // loaded afterwards (see main.cpp)
      if ( guiWarning )
      {
        QString msg = tr( "%1 is not a supported raster data source" ).arg( src );
        if ( !errMsg.isEmpty() )
          msg += '\n' + errMsg;

        visibleMessageBar()->pushMessage( tr( "Unsupported Data Source" ), msg, Qgis::MessageLevel::Critical );
      }
    }
    if ( ! ok )
    {
      returnValue = false;
    }
  }
  return returnValue;
}

QgsPluginLayer *QgisApp::addPluginLayer( const QString &uri, const QString &baseName, const QString &providerKey )
{
  QgsPluginLayer *layer = QgsApplication::pluginLayerRegistry()->createLayer( providerKey, uri );
  if ( !layer )
    return nullptr;

  layer->setName( baseName );

  QgsProject::instance()->addMapLayer( layer );

  return layer;
}



#ifdef ANDROID
void QgisApp::keyReleaseEvent( QKeyEvent *event )
{
  static bool sAccepted = true;
  if ( event->key() == Qt::Key_Close )
  {
    // do something useful here
    int ret = QMessageBox::question( this, tr( "Exit QGIS" ),
                                     tr( "Do you really want to quit QGIS?" ),
                                     QMessageBox::Yes | QMessageBox::No );
    switch ( ret )
    {
      case QMessageBox::Yes:
        this->close();
        break;

      case QMessageBox::No:
        break;
    }
    event->setAccepted( sAccepted ); // don't close my Top Level Widget !
    sAccepted = false;// close the app next time when the user press back button
  }
  else
  {
    QMainWindow::keyReleaseEvent( event );
  }
}
#endif

void QgisApp::keyPressEvent( QKeyEvent *e )
{
  emit keyPressed( e );

#if 0 && defined(QGISDEBUG)
  if ( e->key() == Qt::Key_Backslash && e->modifiers() == Qt::ControlModifier )
  {
    QgsCrashHandler::handle( 0 );
  }
#endif

  //cancel rendering progress with esc key
  if ( e->key() == Qt::Key_Escape )
  {
    stopRendering();
  }
  else
  {
    e->ignore();
  }
}

void QgisApp::newProfile()
{
  QString text = QInputDialog::getText( this, tr( "New profile name" ), tr( "New profile name" ) );
  if ( text.isEmpty() )
    return;

  userProfileManager()->createUserProfile( text );
  userProfileManager()->loadUserProfile( text );
}

void QgisApp::onTaskCompleteShowNotify( long taskId, int status )
{
  if ( status == QgsTask::Complete || status == QgsTask::Terminated )
  {
    long long minTime = QgsSettings().value( QStringLiteral( "minTaskLengthForSystemNotification" ), 5, QgsSettings::App ).toLongLong() * 1000;
    QgsTask *task = QgsApplication::taskManager()->task( taskId );
    if ( task && !( task->flags() & QgsTask::Flag::Hidden ) && task->elapsedTime() >= minTime )
    {
      if ( status == QgsTask::Complete )
        showSystemNotification( tr( "Task complete" ), task->description() );
      else if ( status == QgsTask::Terminated )
        showSystemNotification( tr( "Task failed" ), task->description() );
    }
  }
}

void QgisApp::onTransactionGroupsChanged()
{
  const auto groups = QgsProject::instance()->transactionGroups();
  for ( auto it = groups.constBegin(); it != groups.constEnd(); ++it )
  {
    connect( it.value(), &QgsTransactionGroup::commitError, this, &QgisApp::transactionGroupCommitError, Qt::UniqueConnection );
  }
}

void QgisApp::onSnappingConfigChanged()
{
  mSnappingUtils->setConfig( QgsProject::instance()->snappingConfig() );
}

void QgisApp::createPreviewImage( const QString &path, const QIcon &icon )
{
  // Render the map canvas
  QSize previewSize( 250, 177 ); // h = w / std::sqrt(2)
  QRect previewRect( QPoint( ( mMapCanvas->width() - previewSize.width() ) / 2
                             , ( mMapCanvas->height() - previewSize.height() ) / 2 )
                     , previewSize );

  QPixmap previewImage( previewSize );
  previewImage.fill();
  QPainter previewPainter( &previewImage );
  mMapCanvas->render( &previewPainter, QRect( QPoint(), previewSize ), previewRect );

  if ( !icon.isNull() )
  {
    QPixmap pixmap = icon.pixmap( QSize( 24, 24 ) );
    previewPainter.drawPixmap( QPointF( 250 - 24 - 5, 177 - 24 - 5 ), pixmap );
  }
  previewPainter.end();

  // Save
  previewImage.save( path );
}

void QgisApp::startProfile( const QString &name )
{
  QgsApplication::profiler()->start( name );
}

void QgisApp::endProfile()
{
  QgsApplication::profiler()->end();
}

void QgisApp::functionProfile( void ( QgisApp::*fnc )(), QgisApp *instance, const QString &name )
{
  QgsScopedRuntimeProfile profile( name );
  ( instance->*fnc )();
}

void QgisApp::mapCanvas_keyPressed( QKeyEvent *e )
{
  // Delete selected features when it is possible and KeyEvent was not managed by current MapTool
  if ( ( e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete ) && e->isAccepted() )
  {
    deleteSelected( nullptr, nullptr, true );
  }
}

void QgisApp::customProjection()
{
  showOptionsDialog( this, QStringLiteral( "QgsCustomProjectionOptionsWidget" ) );
}

void QgisApp::newBookmark( bool inProject, const QString &groupName )
{
  QgsBookmark bookmark;
  bookmark.setName( tr( "New bookmark" ) );
  bookmark.setGroup( groupName );
  bookmark.setExtent( QgsReferencedRectangle( mapCanvas()->extent(), mapCanvas()->mapSettings().destinationCrs() ) );
  QgsBookmarkEditorDialog *dlg = new QgsBookmarkEditorDialog( bookmark, inProject, this, mapCanvas() );
  dlg->setAttribute( Qt::WA_DeleteOnClose );
  dlg->show();
}

void QgisApp::showBookmarks()
{
  mBrowserWidget->setUserVisible( true );
  QModelIndex index = browserModel()->findPath( QStringLiteral( "bookmarks:" ) );
  mBrowserWidget->browserWidget()->setActiveIndex( index );
}

void QgisApp::showBookmarkManager( bool show )
{
  mBookMarksDockWidget->setUserVisible( show );
}

QMap<QString, QModelIndex> QgisApp::getBookmarkIndexMap()
{
  return mBookMarksDockWidget->getIndexMap();
}

void QgisApp::zoomToBookmarkIndex( const QModelIndex &index )
{
  mBookMarksDockWidget->zoomToBookmarkIndex( index );
}

QgsMapToolIdentifyAction *QgisApp::identifyMapTool() const
{
  return mMapTools->mapTool< QgsMapToolIdentifyAction >( QgsAppMapTools::Identify );
}

void QgisApp::takeAppScreenShots( const QString &saveDirectory, const int categories )
{
  QgsAppScreenShots ass( saveDirectory );
  ass.takePicturesOf( QgsAppScreenShots::Categories( categories ) );
}

void QgisApp::projectVersionMismatchOccurred( const QString &projectVersion )
{
  const QgsProjectVersion fileVersion( projectVersion );
  const QgsProjectVersion thisVersion( Qgis::version() );

  if ( thisVersion > fileVersion )
  {
    QgsSettings settings;

    if ( settings.value( QStringLiteral( "qgis/warnOldProjectVersion" ), QVariant( true ) ).toBool() )
    {
      QString smalltext = tr( "This project file was saved by QGIS version %1."
                              " When saving this project file, QGIS will update it to version %2, "
                              "possibly rendering it useless for older versions of QGIS." ).arg( projectVersion, Qgis::version() );

      QString title = tr( "Project file is older" );

      visibleMessageBar()->pushMessage( title, smalltext );
    }
  }
  else
  {
    visibleMessageBar()->pushWarning( QString(), tr( "This project file was created by a newer version of QGIS (%1) and could not be completely loaded." ).arg( projectVersion ) );
  }
}

void QgisApp::updateUndoActions()
{
  bool canUndo = false, canRedo = false;
  QgsMapLayer *layer = activeLayer();
  if ( layer  && layer->isEditable() )
  {
    canUndo = layer->undoStack()->canUndo();
    canRedo = layer->undoStack()->canRedo();
  }
  mActionUndo->setEnabled( canUndo );
  mActionRedo->setEnabled( canRedo );
}


// add project directory to python path
void QgisApp::projectChanged( const QDomDocument &doc )
{
  Q_UNUSED( doc )
  QgsProject *project = qobject_cast<QgsProject *>( sender() );
  if ( !project )
    return;

  QFileInfo fi( project->fileName() );
  if ( !fi.exists() )
    return;

  static QString sPrevProjectDir = QString();

  if ( sPrevProjectDir == fi.canonicalPath() )
    return;

  QString expr;
  if ( !sPrevProjectDir.isNull() )
  {
    QString prev = sPrevProjectDir;
    expr = QStringLiteral( "sys.path.remove(u'%1'); " ).arg( prev.replace( '\'', QLatin1String( "\\'" ) ) );
  }

  sPrevProjectDir = fi.canonicalPath();

  QString prev = sPrevProjectDir;
  expr += QStringLiteral( "sys.path.append(u'%1')" ).arg( prev.replace( '\'', QLatin1String( "\\'" ) ) );

  QgsPythonRunner::run( expr );
}

#ifdef HAVE_3D
void QgisApp::write3DMapViewSettings( Qgs3DMapCanvasWidget *widget, QDomDocument &doc, QDomElement &elem3DMap )
{
  QgsReadWriteContext readWriteContext;
  readWriteContext.setPathResolver( QgsProject::instance()->pathResolver() );
  elem3DMap.setAttribute( QStringLiteral( "name" ), widget->canvasName() );
  QDomElement elem3DMapSettings = widget->mapCanvas3D()->map()->writeXml( doc, readWriteContext );
  elem3DMap.appendChild( elem3DMapSettings );
  QDomElement elemCamera = widget->mapCanvas3D()->cameraController()->writeXml( doc );
  elem3DMap.appendChild( elemCamera );
  QDomElement elemAnimation = widget->animationWidget()->animation().writeXml( doc );
  elem3DMap.appendChild( elemAnimation );

  widget->dockableWidgetHelper()->writeXml( elem3DMap );
}

void QgisApp::read3DMapViewSettings( Qgs3DMapCanvasWidget *widget, QDomElement &elem3DMap )
{
  QgsReadWriteContext readWriteContext;
  readWriteContext.setPathResolver( QgsProject::instance()->pathResolver() );

  QDomElement elem3D = elem3DMap.firstChildElement( QStringLiteral( "qgis3d" ) );
  Qgs3DMapSettings *map = new Qgs3DMapSettings;
  map->readXml( elem3D, readWriteContext );
  map->resolveReferences( *QgsProject::instance() );

  map->setTransformContext( QgsProject::instance()->transformContext() );
  map->setPathResolver( QgsProject::instance()->pathResolver() );
  map->setMapThemeCollection( QgsProject::instance()->mapThemeCollection() );
  connect( QgsProject::instance(), &QgsProject::transformContextChanged, map, [map]
  {
    map->setTransformContext( QgsProject::instance()->transformContext() );
  } );

  // these things are not saved in project
  map->setSelectionColor( mMapCanvas->selectionColor() );
  map->setBackgroundColor( mMapCanvas->canvasColor() );
  if ( map->terrainGenerator() && map->terrainGenerator()->type() == QgsTerrainGenerator::Flat )
  {
    QgsFlatTerrainGenerator *flatTerrainGen = static_cast<QgsFlatTerrainGenerator *>( map->terrainGenerator() );
    flatTerrainGen->setExtent( mMapCanvas->projectExtent() );
  }
  map->setOutputDpi( QgsApplication::desktop()->logicalDpiX() );

  widget->setMapSettings( map );

  QDomElement elemCamera = elem3DMap.firstChildElement( QStringLiteral( "camera" ) );
  if ( !elemCamera.isNull() )
  {
    widget->mapCanvas3D()->cameraController()->readXml( elemCamera );
  }

  QDomElement elemAnimation = elem3DMap.firstChildElement( QStringLiteral( "animation3d" ) );
  if ( !elemAnimation.isNull() )
  {
    Qgs3DAnimationSettings animationSettings;
    animationSettings.readXml( elemAnimation );
    widget->animationWidget()->setAnimation( animationSettings );
  }

  widget->dockableWidgetHelper()->readXml( elem3DMap );
}
#endif

void QgisApp::writeProject( QDomDocument &doc )
{
  // QGIS server does not use QgsProject for loading of QGIS project.
  // In order to allow reading of new projects, let's also write the original <legend> tag to the project.
  // Ideally the server should be ported to new layer tree implementation, but that requires
  // non-trivial changes to the server components.
  // The <legend> tag is ignored by QGIS application in >= 2.4 and this way also the new project files
  // can be opened in older versions of QGIS without losing information about layer groups.

  QgsLayerTree *clonedRoot = QgsProject::instance()->layerTreeRoot()->clone();
  QgsLayerTreeUtils::replaceChildrenOfEmbeddedGroups( QgsLayerTree::toGroup( clonedRoot ) );
  QgsLayerTreeUtils::updateEmbeddedGroupsProjectPath( QgsLayerTree::toGroup( clonedRoot ), QgsProject::instance() ); // convert absolute paths to relative paths if required
  QDomElement oldLegendElem = QgsLayerTreeUtils::writeOldLegend( doc, QgsLayerTree::toGroup( clonedRoot ),
                              clonedRoot->hasCustomLayerOrder(), clonedRoot->customLayerOrder() );
  delete clonedRoot;
  QDomElement qgisNode = doc.firstChildElement( QStringLiteral( "qgis" ) );
  qgisNode.appendChild( oldLegendElem );

  QgsProject::instance()->writeEntry( QStringLiteral( "Legend" ), QStringLiteral( "filterByMap" ), static_cast< bool >( layerTreeView()->layerTreeModel()->legendFilterMapSettings() ) );

  if ( QgsProject::instance()->flags() & Qgis::ProjectFlag::RememberAttributeTableWindowsBetweenSessions )
  {
    // save attribute tables
    QDomElement attributeTablesElement = doc.createElement( QStringLiteral( "attributeTables" ) );

    QSet< QgsAttributeTableDialog * > storedDialogs;
    auto saveDialog = [&storedDialogs, &attributeTablesElement, &doc]( QgsAttributeTableDialog * attributeTableDialog )
    {
      if ( storedDialogs.contains( attributeTableDialog ) )
        return;

      QgsDebugMsg( attributeTableDialog->windowTitle() );
      const QDomElement tableElement = attributeTableDialog->writeXml( doc );
      attributeTablesElement.appendChild( tableElement );
      storedDialogs.insert( attributeTableDialog );
    };

    const QList<QWidget * > topLevelWidgets = QgsApplication::topLevelWidgets();
    for ( QWidget *widget : topLevelWidgets )
    {
      QList< QgsAttributeTableDialog * > dialogChildren = widget->findChildren< QgsAttributeTableDialog * >();
      for ( QgsAttributeTableDialog *attributeTableDialog : dialogChildren )
      {
        saveDialog( attributeTableDialog );
      }
    }

    qgisNode.appendChild( attributeTablesElement );
  }

  // Save the position of the map view docks
  QDomElement mapViewNode = doc.createElement( QStringLiteral( "mapViewDocks" ) );
  const auto dockWidgets = findChildren< QgsMapCanvasDockWidget * >();
  for ( QgsMapCanvasDockWidget *w : dockWidgets )
  {
    QDomElement node = doc.createElement( QStringLiteral( "view" ) );
    node.setAttribute( QStringLiteral( "name" ), w->mapCanvas()->objectName() );
    node.setAttribute( QStringLiteral( "synced" ), w->isViewCenterSynchronized() );
    node.setAttribute( QStringLiteral( "showCursor" ), w->isCursorMarkerVisible() );
    node.setAttribute( QStringLiteral( "showExtent" ), w->isMainCanvasExtentVisible() );
    node.setAttribute( QStringLiteral( "scaleSynced" ), w->isViewScaleSynchronized() );
    node.setAttribute( QStringLiteral( "scaleFactor" ), w->scaleFactor() );
    node.setAttribute( QStringLiteral( "showLabels" ), w->labelsVisible() );
    node.setAttribute( QStringLiteral( "zoomSelected" ), w->isAutoZoomToSelected() );
    writeDockWidgetSettings( w, node );
    mapViewNode.appendChild( node );
  }
  qgisNode.appendChild( mapViewNode );

#ifdef HAVE_3D
  QSet<Qgs3DMapCanvasWidget *> openDocks = mOpen3DMapViews;
  for ( Qgs3DMapCanvasWidget *widget : openDocks )
  {
    QString viewName = widget->canvasName();
    QDomElement elem3DMap = doc.createElement( QStringLiteral( "view" ) );
    elem3DMap.setAttribute( QStringLiteral( "isOpen" ), 1 );
    write3DMapViewSettings( widget, doc, elem3DMap );
    QgsProject::instance()->viewsManager()->register3DViewSettings( viewName, elem3DMap );
  }
#endif
  projectChanged( doc );
}

void QgisApp::writeDockWidgetSettings( QDockWidget *dockWidget, QDomElement &elem )
{
  elem.setAttribute( QStringLiteral( "x" ), dockWidget->x() );
  elem.setAttribute( QStringLiteral( "y" ), dockWidget->y() );
  elem.setAttribute( QStringLiteral( "width" ), dockWidget->width() );
  elem.setAttribute( QStringLiteral( "height" ), dockWidget->height() );
  elem.setAttribute( QStringLiteral( "floating" ), dockWidget->isFloating() );
  elem.setAttribute( QStringLiteral( "area" ), dockWidgetArea( dockWidget ) );
}

bool QgisApp::askUserForDatumTransform( const QgsCoordinateReferenceSystem &sourceCrs, const QgsCoordinateReferenceSystem &destinationCrs, const QgsMapLayer *layer )
{
  Q_ASSERT( qApp->thread() == QThread::currentThread() );

  QString title;
  if ( layer )
  {
    // try to make a user-friendly (short!) identifier for the layer
    QString layerIdentifier;
    if ( !layer->name().isEmpty() )
    {
      layerIdentifier = layer->name();
    }
    else
    {
      const QVariantMap parts = QgsProviderRegistry::instance()->decodeUri( layer->providerType(), layer->source() );
      if ( parts.contains( QStringLiteral( "path" ) ) )
      {
        const QFileInfo fi( parts.value( QStringLiteral( "path" ) ).toString() );
        layerIdentifier = fi.fileName();
      }
      else if ( layer->dataProvider() )
      {
        const QgsDataSourceUri uri( layer->source() );
        layerIdentifier = uri.table();
      }
    }
    if ( !layerIdentifier.isEmpty() )
      title = tr( "Select Transformation for %1" ).arg( layerIdentifier );
  }

  return QgsDatumTransformDialog::run( sourceCrs, destinationCrs, this, mMapCanvas, title );
}

void QgisApp::readDockWidgetSettings( QDockWidget *dockWidget, const QDomElement &elem )
{
  int x = elem.attribute( QStringLiteral( "x" ), QStringLiteral( "0" ) ).toInt();
  int y = elem.attribute( QStringLiteral( "y" ), QStringLiteral( "0" ) ).toInt();
  int w = elem.attribute( QStringLiteral( "width" ), QStringLiteral( "400" ) ).toInt();
  int h = elem.attribute( QStringLiteral( "height" ), QStringLiteral( "400" ) ).toInt();
  bool floating = elem.attribute( QStringLiteral( "floating" ), QStringLiteral( "0" ) ).toInt();
  Qt::DockWidgetArea area = static_cast< Qt::DockWidgetArea >( elem.attribute( QStringLiteral( "area" ), QString::number( Qt::RightDockWidgetArea ) ).toInt() );

  setupDockWidget( dockWidget, floating, QRect( x, y, w, h ), area );
}


void QgisApp::readProject( const QDomDocument &doc )
{
  projectChanged( doc );

  // force update of canvas, without automatic changes to extent and OTF projections
  bool autoSetupOnFirstLayer = mLayerTreeCanvasBridge->autoSetupOnFirstLayer();
  mLayerTreeCanvasBridge->setAutoSetupOnFirstLayer( false );

  mLayerTreeCanvasBridge->setCanvasLayers();

  if ( autoSetupOnFirstLayer )
    mLayerTreeCanvasBridge->setAutoSetupOnFirstLayer( true );

  QDomNodeList nodes = doc.elementsByTagName( QStringLiteral( "mapViewDocks" ) );
  QList< QgsMapCanvas * > views;
  if ( !nodes.isEmpty() )
  {
    QDomNode viewNode = nodes.at( 0 );
    nodes = viewNode.childNodes();
    for ( int i = 0; i < nodes.size(); ++i )
    {
      QDomElement elementNode = nodes.at( i ).toElement();
      QString mapName = elementNode.attribute( QStringLiteral( "name" ) );
      bool synced = elementNode.attribute( QStringLiteral( "synced" ), QStringLiteral( "0" ) ).toInt();
      bool showCursor = elementNode.attribute( QStringLiteral( "showCursor" ), QStringLiteral( "0" ) ).toInt();
      bool showExtent = elementNode.attribute( QStringLiteral( "showExtent" ), QStringLiteral( "0" ) ).toInt();
      bool scaleSynced = elementNode.attribute( QStringLiteral( "scaleSynced" ), QStringLiteral( "0" ) ).toInt();
      double scaleFactor = elementNode.attribute( QStringLiteral( "scaleFactor" ), QStringLiteral( "1" ) ).toDouble();
      bool showLabels = elementNode.attribute( QStringLiteral( "showLabels" ), QStringLiteral( "1" ) ).toInt();
      bool zoomSelected = elementNode.attribute( QStringLiteral( "zoomSelected" ), QStringLiteral( "0" ) ).toInt();

      QgsMapCanvasDockWidget *mapCanvasDock = createNewMapCanvasDock( mapName );
      readDockWidgetSettings( mapCanvasDock, elementNode );
      QgsMapCanvas *mapCanvas = mapCanvasDock->mapCanvas();
      mapCanvasDock->setViewCenterSynchronized( synced );
      mapCanvasDock->setCursorMarkerVisible( showCursor );
      mapCanvasDock->setScaleFactor( scaleFactor );
      mapCanvasDock->setViewScaleSynchronized( scaleSynced );
      mapCanvasDock->setMainCanvasExtentVisible( showExtent );
      mapCanvasDock->setLabelsVisible( showLabels );
      mapCanvasDock->setAutoZoomToSelected( zoomSelected );
      mapCanvas->readProject( doc );
      views << mapCanvas;
    }
  }

#ifdef HAVE_3D
  // Open 3D Views that were already open
  for ( QDomElement viewConfig : QgsProject::instance()->viewsManager()->get3DViews() )
  {
    QString viewName = viewConfig.attribute( QStringLiteral( "name" ) );
    bool isOpen = viewConfig.attribute( QStringLiteral( "isOpen" ), QStringLiteral( "1" ) ).toInt() == 1;
    if ( !isOpen )
      continue;
    bool isDocked = viewConfig.attribute( QStringLiteral( "isDocked" ), "1" ).toInt() == 1;
    Qgs3DMapCanvasWidget *mapCanvas3D = createNew3DMapCanvasDock( viewName, isDocked );
    read3DMapViewSettings( mapCanvas3D, viewConfig );
  }
#endif

  if ( QgsProject::instance()->flags() & Qgis::ProjectFlag::RememberAttributeTableWindowsBetweenSessions )
  {
    // restore attribute tables
    const QDomElement attributeTablesElement = doc.documentElement().firstChildElement( QStringLiteral( "attributeTables" ) );
    const QDomNodeList attributeTableNodes = attributeTablesElement.elementsByTagName( QStringLiteral( "attributeTable" ) );
    for ( int i = 0; i < attributeTableNodes.size(); ++i )
    {
      const QDomElement attributeTableElement = attributeTableNodes.at( i ).toElement();
      const QString layerId = attributeTableElement.attribute( QStringLiteral( "layer" ) );
      if ( QgsVectorLayer *layer = qobject_cast< QgsVectorLayer * >( QgsProject::instance()->mapLayer( layerId ) ) )
      {
        if ( layer->isValid() )
        {
          bool initiallyDocked = attributeTableElement.attribute( QStringLiteral( "isDocked" ), QStringLiteral( "0" ) ).toInt() == 1;
          const QgsAttributeTableFilterModel::FilterMode filterMode = qgsEnumKeyToValue( attributeTableElement.attribute( QStringLiteral( "filterMode" ) ), QgsAttributeTableFilterModel::ShowAll );

          QgsAttributeTableDialog *dialog = new QgsAttributeTableDialog( layer, filterMode, nullptr, Qt::Window, &initiallyDocked );
          dialog->readXml( attributeTableElement );
        }
      }
    }
  }

  // unfreeze all new views at once. We don't do this as they are created since additional
  // views which may exist in project could rearrange the docks and cause the canvases to resize
  // resulting in multiple redraws
  const auto constViews = views;
  for ( QgsMapCanvas *c : constViews )
  {
    c->freeze( false );
  }
}

void QgisApp::showLayerProperties( QgsMapLayer *mapLayer, const QString &page )
{
  /*
  TODO: Consider reusing the property dialogs again.
  Sometimes around mid 2005, the property dialogs were saved for later reuse;
  this resulted in a time savings when reopening the dialog. The code below
  cannot be used as is, however, simply by saving the dialog pointer here.
  Either the map layer needs to be passed as an argument to sync or else
  a separate copy of the dialog pointer needs to be stored with each layer.
  */

  if ( !mapLayer )
    return;

  if ( !QgsProject::instance()->layerIsEmbedded( mapLayer->id() ).isEmpty() )
  {
    return; //don't show properties of embedded layers
  }

  // collect factories from registered data providers
  QList<const QgsMapLayerConfigWidgetFactory *> providerFactories = QgsGui::providerGuiRegistry()->mapLayerConfigWidgetFactories( mapLayer );
  providerFactories.append( mMapLayerPanelFactories );

  switch ( mapLayer->type() )
  {
    case QgsMapLayerType::RasterLayer:
    {
      QgsRasterLayerProperties *rasterLayerPropertiesDialog = new QgsRasterLayerProperties( mapLayer, mMapCanvas, this );

      for ( const QgsMapLayerConfigWidgetFactory *factory : std::as_const( providerFactories ) )
      {
        rasterLayerPropertiesDialog->addPropertiesPageFactory( factory );
      }

      if ( !page.isEmpty() )
        rasterLayerPropertiesDialog->setCurrentPage( page );
      else
        rasterLayerPropertiesDialog->restoreLastPage();

      // Cannot use exec here due to raster transparency map tool:
      // in order to pass focus to the canvas, the dialog needs to
      // be hidden and shown in non-modal mode.
      rasterLayerPropertiesDialog->setModal( true );
      rasterLayerPropertiesDialog->show();
      // Delete (later, for safety) since dialog cannot be reused without
      // updating code
      connect( rasterLayerPropertiesDialog, &QgsRasterLayerProperties::accepted, [ rasterLayerPropertiesDialog ]
      {
        rasterLayerPropertiesDialog->deleteLater();
      } );
      connect( rasterLayerPropertiesDialog, &QgsRasterLayerProperties::rejected, [ rasterLayerPropertiesDialog ]
      {
        rasterLayerPropertiesDialog->deleteLater();
      } );
      break;
    }

    case QgsMapLayerType::MeshLayer:
    {
      QgsMeshLayerProperties meshLayerPropertiesDialog( mapLayer, mMapCanvas, this );

      for ( const QgsMapLayerConfigWidgetFactory *factory : std::as_const( providerFactories ) )
      {
        meshLayerPropertiesDialog.addPropertiesPageFactory( factory );
      }

      if ( !page.isEmpty() )
        meshLayerPropertiesDialog.setCurrentPage( page );
      else
        meshLayerPropertiesDialog.restoreLastPage();

      mMapStyleWidget->blockUpdates( true );
      if ( meshLayerPropertiesDialog.exec() )
      {
        activateDeactivateLayerRelatedActions( mapLayer );
        mMapStyleWidget->updateCurrentWidgetLayer();
      }
      mMapStyleWidget->blockUpdates( false ); // delete since dialog cannot be reused without updating code
      break;
    }

    case QgsMapLayerType::VectorLayer:
    {
      QgsVectorLayer *vlayer = qobject_cast<QgsVectorLayer *>( mapLayer );

      QgsVectorLayerProperties *vectorLayerPropertiesDialog = new QgsVectorLayerProperties( mMapCanvas, visibleMessageBar(), vlayer, this );
      connect(
        vectorLayerPropertiesDialog, static_cast<void ( QgsVectorLayerProperties::* )( QgsMapLayer * )>( &QgsVectorLayerProperties::toggleEditing ),
      this, [ = ]( QgsMapLayer * layer ) { toggleEditing( layer ); }
      );
      connect( vectorLayerPropertiesDialog, &QgsVectorLayerProperties::exportAuxiliaryLayer, this, [ = ]( QgsAuxiliaryLayer * layer )
      {
        if ( layer )
        {
          std::unique_ptr<QgsVectorLayer> clone;
          clone.reset( layer->toSpatialLayer() );

          saveAsFile( clone.get() );
        }
      } );
      for ( const QgsMapLayerConfigWidgetFactory *factory : std::as_const( providerFactories ) )
      {
        vectorLayerPropertiesDialog->addPropertiesPageFactory( factory );
      }

      if ( !page.isEmpty() )
        vectorLayerPropertiesDialog->setCurrentPage( page );
      else
        vectorLayerPropertiesDialog->restoreLastPage();

      mMapStyleWidget->blockUpdates( true );
      if ( vectorLayerPropertiesDialog->exec() )
      {
        activateDeactivateLayerRelatedActions( mapLayer );
        mMapStyleWidget->updateCurrentWidgetLayer();
      }
      mMapStyleWidget->blockUpdates( false );

      delete vectorLayerPropertiesDialog; // delete since dialog cannot be reused without updating code
      break;
    }

    case QgsMapLayerType::VectorTileLayer:
    {
      QgsVectorTileLayerProperties vectorTileLayerPropertiesDialog( qobject_cast<QgsVectorTileLayer *>( mapLayer ), mMapCanvas, visibleMessageBar(), this );
      if ( !page.isEmpty() )
        vectorTileLayerPropertiesDialog.setCurrentPage( page );
      else
        vectorTileLayerPropertiesDialog.restoreLastPage();

      mMapStyleWidget->blockUpdates( true );
      if ( vectorTileLayerPropertiesDialog.exec() )
      {
        activateDeactivateLayerRelatedActions( mapLayer );
        mMapStyleWidget->updateCurrentWidgetLayer();
      }
      mMapStyleWidget->blockUpdates( false ); // delete since dialog cannot be reused without updating code
      break;
    }

    case QgsMapLayerType::PointCloudLayer:
    {
      QgsPointCloudLayerProperties pointCloudLayerPropertiesDialog( qobject_cast<QgsPointCloudLayer *>( mapLayer ), mMapCanvas, visibleMessageBar(), this );

      if ( !page.isEmpty() )
        pointCloudLayerPropertiesDialog.setCurrentPage( page );
      else
        pointCloudLayerPropertiesDialog.restoreLastPage();

      for ( const QgsMapLayerConfigWidgetFactory *factory : std::as_const( providerFactories ) )
      {
        pointCloudLayerPropertiesDialog.addPropertiesPageFactory( factory );
      }

      mMapStyleWidget->blockUpdates( true );
      if ( pointCloudLayerPropertiesDialog.exec() )
      {
        activateDeactivateLayerRelatedActions( mapLayer );
        mMapStyleWidget->updateCurrentWidgetLayer();
      }
      mMapStyleWidget->blockUpdates( false ); // delete since dialog cannot be reused without updating code
      break;
    }

    case QgsMapLayerType::PluginLayer:
    {
      QgsPluginLayer *pl = qobject_cast<QgsPluginLayer *>( mapLayer );
      if ( !pl )
        return;

      QgsPluginLayerType *plt = QgsApplication::pluginLayerRegistry()->pluginLayerType( pl->pluginLayerType() );
      if ( !plt )
        return;

      if ( !plt->showLayerProperties( pl ) )
      {
        visibleMessageBar()->pushMessage( tr( "Warning" ),
                                          tr( "This layer doesn't have a properties dialog." ),
                                          Qgis::MessageLevel::Info );
      }
      break;
    }

    case QgsMapLayerType::AnnotationLayer:
    {
      QgsAnnotationLayerProperties annotationLayerPropertiesDialog( qobject_cast<QgsAnnotationLayer *>( mapLayer ), mMapCanvas, visibleMessageBar(), this );

      if ( !page.isEmpty() )
        annotationLayerPropertiesDialog.setCurrentPage( page );
      else
        annotationLayerPropertiesDialog.restoreLastPage();

      for ( const QgsMapLayerConfigWidgetFactory *factory : std::as_const( providerFactories ) )
      {
        annotationLayerPropertiesDialog.addPropertiesPageFactory( factory );
      }

      mMapStyleWidget->blockUpdates( true );
      if ( annotationLayerPropertiesDialog.exec() )
      {
        activateDeactivateLayerRelatedActions( mapLayer );
        mMapStyleWidget->updateCurrentWidgetLayer();
      }
      mMapStyleWidget->blockUpdates( false ); // delete since dialog cannot be reused without updating code
      break;
    }

    case QgsMapLayerType::GroupLayer:
      break;
  }
}

void QgisApp::namSetup()
{
  QgsNetworkAccessManager *nam = QgsNetworkAccessManager::instance();

  connect( nam, &QNetworkAccessManager::proxyAuthenticationRequired,
           this, &QgisApp::namProxyAuthenticationRequired );

  connect( nam, qOverload< QgsNetworkRequestParameters >( &QgsNetworkAccessManager::requestTimedOut ),
           this, &QgisApp::namRequestTimedOut );

  nam->setAuthHandler( std::make_unique<QgsAppAuthRequestHandler>() );
#ifndef QT_NO_SSL
  nam->setSslErrorHandler( std::make_unique<QgsAppSslErrorHandler>() );
#endif
}

void QgisApp::namProxyAuthenticationRequired( const QNetworkProxy &proxy, QAuthenticator *auth )
{
  QgsSettings settings;
  if ( !settings.value( QStringLiteral( "proxy/proxyEnabled" ), false ).toBool() ||
       settings.value( QStringLiteral( "proxy/proxyType" ), "" ).toString() == QLatin1String( "DefaultProxy" ) )
  {
    auth->setUser( QString() );
    return;
  }

  QString username = auth->user();
  QString password = auth->password();

  for ( ;; )
  {
    bool ok = QgsCredentials::instance()->get(
                QStringLiteral( "proxy %1:%2 [%3]" ).arg( proxy.hostName() ).arg( proxy.port() ).arg( auth->realm() ),
                username, password,
                tr( "Proxy authentication required" ) );
    if ( !ok )
      return;

    if ( auth->user() != username || ( password != auth->password() && !password.isNull() ) )
    {
      QgsCredentials::instance()->put(
        QStringLiteral( "proxy %1:%2 [%3]" ).arg( proxy.hostName() ).arg( proxy.port() ).arg( auth->realm() ),
        username, password
      );
      break;
    }
    else
    {
      // credentials didn't change - stored ones probably wrong? clear password and retry
      QgsCredentials::instance()->put(
        QStringLiteral( "proxy %1:%2 [%3]" ).arg( proxy.hostName() ).arg( proxy.port() ).arg( auth->realm() ),
        username, QString() );
    }
  }

  auth->setUser( username );
  auth->setPassword( password );
}

void QgisApp::namRequestTimedOut( const QgsNetworkRequestParameters &request )
{
  QLabel *msgLabel = new QLabel( tr( "Network request to %1 timed out, any data received is likely incomplete." ).arg( request.request().url().host() ) +
                                 tr( " Please check the <a href=\"#messageLog\">message log</a> for further info." ), messageBar() );
  msgLabel->setWordWrap( true );
  connect( msgLabel, &QLabel::linkActivated, mLogDock, &QWidget::show );
  messageBar()->pushItem( new QgsMessageBarItem( msgLabel, Qgis::MessageLevel::Warning, QgsMessageBar::defaultMessageTimeout() ) );
}

void QgisApp::namUpdate()
{
  QgsNetworkAccessManager::instance()->setupDefaultProxyAndCache();
}

void QgisApp::masterPasswordSetup()
{
  connect( QgsApplication::authManager(), &QgsAuthManager::messageOut,
           this, &QgisApp::authMessageOut );
  connect( QgsApplication::authManager(), &QgsAuthManager::passwordHelperMessageOut,
           this, &QgisApp::authMessageOut );
  connect( QgsApplication::authManager(), &QgsAuthManager::authDatabaseEraseRequested,
           this, &QgisApp::eraseAuthenticationDatabase );
}

void QgisApp::eraseAuthenticationDatabase()
{
  // First check if now is a good time to interact with the user, e.g. project is done loading.
  // If not, ask QgsAuthManager to re-emit authDatabaseEraseRequested from the schedule timer.
  // No way to know if user interaction will interfere with plugins loading layers.

  if ( !QgsProject::instance()->fileName().isNull() ) // a non-blank project is loaded
  {
    // Apparently, as of QGIS 2.9, the only way to query that the project is in a
    // layer-loading state is via a custom property of the project's layer tree.
    QgsLayerTreeGroup *layertree( QgsProject::instance()->layerTreeRoot() );
    if ( layertree && layertree->customProperty( QStringLiteral( "loading" ) ).toBool() )
    {
      QgsDebugMsgLevel( QStringLiteral( "Project loading, skipping auth db erase" ), 2 );
      QgsApplication::authManager()->setScheduledAuthDatabaseEraseRequestEmitted( false );
      return;
    }
  }

  // TODO: Check if Browser panel is also still loading?
  //       It has auto-connections in parallel (if tree item is expanded), though
  //       such connections with possible master password requests *should* be ignored
  //       when there is an authentication db erase scheduled.

  // This function should tell QgsAuthManager to stop any erase db schedule timer,
  // *after* interacting with the user
  QgsAuthGuiUtils::eraseAuthenticationDatabase( messageBar(), this );
}

void QgisApp::authMessageOut( const QString &message, const QString &authtag, QgsAuthManager::MessageLevel level )
{
  // Use system notifications if the main window is not the active one,
  // push message to the message bar if the main window is active
  if ( qApp->activeWindow() != this )
  {
    showSystemNotification( tr( "QGIS Authentication" ), message );
  }
  else
  {
    int levelint = static_cast< int >( level );
    visibleMessageBar()->pushMessage( authtag, message, static_cast< Qgis::MessageLevel >( levelint ) );
  }
}

void QgisApp::completeInitialization()
{
  emit initializationCompleted();
}

void QgisApp::toolButtonActionTriggered( QAction *action )
{
  QToolButton *bt = qobject_cast<QToolButton *>( sender() );
  if ( !bt )
    return;

  QgsSettings settings;
  if ( action == mActionSelectFeatures )
    settings.setValue( QStringLiteral( "UI/selectTool" ), 1 );
  else if ( action == mActionSelectRadius )
    settings.setValue( QStringLiteral( "UI/selectTool" ), 2 );
  else if ( action == mActionSelectPolygon )
    settings.setValue( QStringLiteral( "UI/selectTool" ), 3 );
  else if ( action == mActionSelectFreehand )
    settings.setValue( QStringLiteral( "UI/selectTool" ), 4 );
  else if ( action == mActionSelectByForm )
    settings.setValue( QStringLiteral( "UI/selectionTool" ), 0 );
  else if ( action == mActionSelectByExpression )
    settings.setValue( QStringLiteral( "UI/selectionTool" ), 1 );
  else if ( action == mActionSelectAll )
    settings.setValue( QStringLiteral( "UI/selectionTool" ), 2 );
  else if ( action == mActionInvertSelection )
    settings.setValue( QStringLiteral( "UI/selectionTool" ), 3 );
  else if ( action == mActionDeselectAll )
    settings.setValue( QStringLiteral( "UI/deselectionTool" ), 0 );
  else if ( action == mActionDeselectActiveLayer )
    settings.setValue( QStringLiteral( "UI/deselectionTool" ), 1 );
  else if ( action == mActionOpenTable )
    settings.setValue( QStringLiteral( "UI/openTableTool" ), 0 );
  else if ( action == mActionOpenTableSelected )
    settings.setValue( QStringLiteral( "UI/openTableTool" ), 1 );
  else if ( action == mActionOpenTableVisible )
    settings.setValue( QStringLiteral( "UI/openTableTool" ), 2 );
  else if ( action == mActionOpenTableEdited )
    settings.setValue( QStringLiteral( "UI/openTableTool" ), 3 );
  else if ( action == mActionMeasure )
    settings.setValue( QStringLiteral( "UI/measureTool" ), 0 );
  else if ( action == mActionMeasureArea )
    settings.setValue( QStringLiteral( "UI/measureTool" ), 1 );
  else if ( action == mActionMeasureAngle )
    settings.setValue( QStringLiteral( "UI/measureTool" ), 2 );
  else if ( action == mActionTextAnnotation )
    settings.setValue( QStringLiteral( "UI/annotationTool" ), 0 );
  else if ( action == mActionFormAnnotation )
    settings.setValue( QStringLiteral( "UI/annotationTool" ), 1 );
  else if ( action == mActionHtmlAnnotation )
    settings.setValue( QStringLiteral( "UI/annotationTool" ), 2 );
  else if ( action == mActionSvgAnnotation )
    settings.setValue( QStringLiteral( "UI/annotationTool" ), 3 );
  else if ( action == mActionAnnotation )
    settings.setValue( QStringLiteral( "UI/annotationTool" ), 4 );
  else if ( action == mActionNewSpatiaLiteLayer )
    settings.setValue( QStringLiteral( "UI/defaultNewLayer" ), 0 );
  else if ( action == mActionNewVectorLayer )
    settings.setValue( QStringLiteral( "UI/defaultNewLayer" ), 1 );
  else if ( action == mActionNewMemoryLayer )
    settings.setValue( QStringLiteral( "UI/defaultNewLayer" ), 2 );
  else if ( action == mActionNewGeoPackageLayer )
    settings.setValue( QStringLiteral( "UI/defaultNewLayer" ), 3 );
  else if ( action == mActionRotatePointSymbols )
    settings.setValue( QStringLiteral( "UI/defaultPointSymbolAction" ), 0 );
  else if ( action == mActionOffsetPointSymbol )
    settings.setValue( QStringLiteral( "UI/defaultPointSymbolAction" ), 1 );
  else if ( mActionAddPgLayer && action == mActionAddPgLayer )
    settings.setValue( QStringLiteral( "UI/defaultAddDbLayerAction" ), 0 );
  else if ( mActionAddMssqlLayer && action == mActionAddMssqlLayer )
    settings.setValue( QStringLiteral( "UI/defaultAddDbLayerAction" ), 1 );
  else if ( mActionAddOracleLayer && action == mActionAddOracleLayer )
    settings.setValue( QStringLiteral( "UI/defaultAddDbLayerAction" ), 2 );
  else if ( mActionAddHanaLayer && action == mActionAddHanaLayer )
    settings.setValue( QStringLiteral( "UI/defaultAddDbLayerAction" ), 3 );
  else if ( action == mActionMoveFeature )
    settings.setValue( QStringLiteral( "UI/defaultMoveTool" ), 0 );
  else if ( action == mActionMoveFeatureCopy )
    settings.setValue( QStringLiteral( "UI/defaultMoveTool" ), 1 );
  else if ( action == mActionVertexTool )
    settings.setEnumValue( QStringLiteral( "UI/defaultVertexTool" ), QgsVertexTool::AllLayers );
  else if ( action == mActionVertexToolActiveLayer )
    settings.setEnumValue( QStringLiteral( "UI/defaultVertexTool" ), QgsVertexTool::ActiveLayer );

  bt->setDefaultAction( action );
}

QMenu *QgisApp::createPopupMenu()
{
  QMenu *menu = QMainWindow::createPopupMenu();
  QList< QAction * > al = menu->actions();
  QList< QAction * > panels, toolbars;

  if ( !al.isEmpty() )
  {
    bool found = false;
    for ( int i = 0; i < al.size(); ++i )
    {
      if ( al[ i ]->isSeparator() )
      {
        found = true;
        continue;
      }

      if ( !found )
      {
        panels.append( al[ i ] );
      }
      else
      {
        toolbars.append( al[ i ] );
      }
    }

    std::sort( panels.begin(), panels.end(), cmpByText_ );
    QWidgetAction *panelstitle = new QWidgetAction( menu );
    QLabel *plabel = new QLabel( QStringLiteral( "<b>%1</b>" ).arg( tr( "Panels" ) ) );
    plabel->setMargin( 3 );
    plabel->setAlignment( Qt::AlignHCenter );
    panelstitle->setDefaultWidget( plabel );
    menu->addAction( panelstitle );
    const auto constPanels = panels;
    for ( QAction *a : constPanels )
    {
      if ( !a->property( "fixed_title" ).toBool() )
      {
        // append " Panel" to menu text. Only ever do this once, because the actions are not unique to
        // this single popup menu

        a->setText( tr( "%1 Panel" ).arg( a->text() ) );
        a->setProperty( "fixed_title", true );
      }
      menu->addAction( a );
    }
    menu->addSeparator();
    QWidgetAction *toolbarstitle = new QWidgetAction( menu );
    QLabel *tlabel = new QLabel( QStringLiteral( "<b>%1</b>" ).arg( tr( "Toolbars" ) ) );
    tlabel->setMargin( 3 );
    tlabel->setAlignment( Qt::AlignHCenter );
    toolbarstitle->setDefaultWidget( tlabel );
    menu->addAction( toolbarstitle );
    std::sort( toolbars.begin(), toolbars.end(), cmpByText_ );
    const auto constToolbars = toolbars;
    for ( QAction *a : constToolbars )
    {
      menu->addAction( a );
    }
  }

  return menu;
}


void QgisApp::showSystemNotification( const QString &title, const QString &message, bool replaceExisting )
{
  static QVariant sLastMessageId;

  QgsNative::NotificationSettings settings;
  settings.transient = true;
  if ( replaceExisting )
    settings.messageId = sLastMessageId;
  settings.svgAppIconPath = QgsApplication::iconsPath() + QStringLiteral( "qgis_icon.svg" );
  settings.pngAppIconPath = QgsApplication::appIconPath();

  QgsNative::NotificationResult result = QgsGui::nativePlatformInterface()->showDesktopNotification( title, message, settings );

  if ( !result.successful )
  {
    // fallback - use message bar if available, otherwise use a message log
    if ( auto *lMessageBar = messageBar() )
    {
      lMessageBar->pushInfo( title, message );
    }
    else
    {
      QgsMessageLog::logMessage( QStringLiteral( "%1: %2" ).arg( title, message ) );
    }
  }
  else
  {
    sLastMessageId = result.messageId;
  }
}

void QgisApp::onLayerError( const QString &msg )
{
  QgsMapLayer *layer = qobject_cast<QgsMapLayer *>( sender() );

  Q_ASSERT( layer );

  visibleMessageBar()->pushCritical( tr( "Layer %1" ).arg( layer->name() ), msg );
}

bool QgisApp::gestureEvent( QGestureEvent *event )
{
#ifdef Q_OS_ANDROID
  if ( QGesture *tapAndHold = event->gesture( Qt::TapAndHoldGesture ) )
  {
    tapAndHoldTriggered( static_cast<QTapAndHoldGesture *>( tapAndHold ) );
  }
  return true;
#else
  Q_UNUSED( event )
  return false;
#endif
}

void QgisApp::tapAndHoldTriggered( QTapAndHoldGesture *gesture )
{
  if ( gesture->state() == Qt::GestureFinished )
  {
    QPoint pos = gesture->position().toPoint();
    QWidget *receiver = QApplication::widgetAt( pos );

    QApplication::postEvent( receiver, new QMouseEvent( QEvent::MouseButtonPress, receiver->mapFromGlobal( pos ), Qt::RightButton, Qt::RightButton, Qt::NoModifier ) );
    QApplication::postEvent( receiver, new QMouseEvent( QEvent::MouseButtonRelease, receiver->mapFromGlobal( pos ), Qt::RightButton, Qt::RightButton, Qt::NoModifier ) );
  }
}

void QgisApp::transactionGroupCommitError( const QString &error )
{
  displayMessage( tr( "Transaction" ), error, Qgis::MessageLevel::Critical );
}

QgsFeature QgisApp::duplicateFeatures( QgsMapLayer *mlayer, const QgsFeature &feature )
{
  if ( mlayer->type() != QgsMapLayerType::VectorLayer )
    return QgsFeature();

  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( mlayer );

  if ( !layer->isEditable() )
  {
    //should never happen because the action should be disabled
    QString msg = tr( "Cannot duplicate feature in not editable mode on layer %1" ).arg( layer->name() );
    visibleMessageBar()->pushMessage( msg, Qgis::MessageLevel::Warning );
    return QgsFeature();
  }

  QgsFeatureList featureList;

  if ( feature.isValid() )
  {
    featureList.append( feature );
  }
  else
  {
    featureList.append( layer->selectedFeatures() );
  }

  int featureCount = 0;

  QString childrenInfo;

  for ( const QgsFeature &f : featureList )
  {
    QgsVectorLayerUtils::QgsDuplicateFeatureContext duplicateFeatureContext;

    QgsVectorLayerUtils::duplicateFeature( layer, f, QgsProject::instance(), duplicateFeatureContext );
    featureCount += 1;

    const auto duplicatedFeatureContextLayers = duplicateFeatureContext.layers();
    for ( QgsVectorLayer *chl : duplicatedFeatureContextLayers )
    {
      childrenInfo += ( tr( "%1 children on layer %2 duplicated" ).arg( QLocale().toString( duplicateFeatureContext.duplicatedFeatures( chl ).size() ), chl->name() ) );
    }
  }

  visibleMessageBar()->pushMessage( tr( "%1 features on layer %2 duplicated\n%3" ).arg( QLocale().toString( featureCount ), layer->name(), childrenInfo ), Qgis::MessageLevel::Success );

  return QgsFeature();
}


QgsFeature QgisApp::duplicateFeatureDigitized( QgsMapLayer *mlayer, const QgsFeature &feature )
{
  if ( mlayer->type() != QgsMapLayerType::VectorLayer )
    return QgsFeature();

  QgsVectorLayer *layer = qobject_cast<QgsVectorLayer *>( mlayer );

  if ( !layer->isEditable() )
  {
    //should never happen because the action should be disabled
    QString msg = tr( "Cannot duplicate feature in not editable mode on layer %1" ).arg( layer->name() );
    visibleMessageBar()->pushMessage( msg, Qgis::MessageLevel::Warning );
    return QgsFeature();
  }

  QgsMapToolDigitizeFeature *digitizeFeature = new QgsMapToolDigitizeFeature( mMapCanvas, mAdvancedDigitizingDockWidget, QgsMapToolCapture::CaptureNone );
  digitizeFeature->setLayer( layer );

  mMapCanvas->setMapTool( digitizeFeature );
  mMapCanvas->window()->raise();
  mMapCanvas->activateWindow();
  mMapCanvas->setFocus();

  QString msg = tr( "Digitize the duplicate on layer %1" ).arg( layer->name() );
  visibleMessageBar()->pushMessage( msg, Qgis::MessageLevel::Info );

  connect( digitizeFeature, static_cast<void ( QgsMapToolDigitizeFeature::* )( const QgsFeature & )>( &QgsMapToolDigitizeFeature::digitizingCompleted ), this, [this, layer, feature, digitizeFeature]( const QgsFeature & digitizedFeature )
  {
    QString msg = tr( "Duplicate digitized" );
    visibleMessageBar()->pushMessage( msg, Qgis::MessageLevel::Info );

    QgsVectorLayerUtils::QgsDuplicateFeatureContext duplicateFeatureContext;

    QgsFeature newFeature = feature;
    newFeature.setGeometry( digitizedFeature.geometry() );
    QgsVectorLayerUtils::duplicateFeature( layer, newFeature, QgsProject::instance(), duplicateFeatureContext );

    QString childrenInfo;
    const auto duplicateFeatureContextLayers = duplicateFeatureContext.layers();
    for ( QgsVectorLayer *chl : duplicateFeatureContextLayers )
    {
      childrenInfo += ( tr( "%n children on layer %1 duplicated", nullptr, duplicateFeatureContext.duplicatedFeatures( chl ).size() ).arg( chl->name() ) );
    }

    visibleMessageBar()->pushMessage( tr( "Feature on layer %1 duplicated\n%2" ).arg( layer->name(), childrenInfo ), Qgis::MessageLevel::Success );

    mMapCanvas->unsetMapTool( digitizeFeature );
  }
         );

  connect( digitizeFeature, static_cast<void ( QgsMapToolDigitizeFeature::* )()>( &QgsMapToolDigitizeFeature::digitizingFinished ), this, [digitizeFeature]()
  {
    digitizeFeature->deleteLater();
  }
         );

  return QgsFeature();
}

void QgisApp::populateProjectStorageMenu( QMenu *menu, const bool saving )
{
  menu->clear();

  if ( saving )
  {
    QAction *action = menu->addAction( tr( "Templates" ) + QChar( 0x2026 ) ); // 0x2026 = ellipsis character
    connect( action, &QAction::triggered, this, [ this ]
    {
      QgsSettings settings;
      QString templateDirName = settings.value( QStringLiteral( "qgis/projectTemplateDir" ),
          QString( QgsApplication::qgisSettingsDirPath() + "project_templates" ) ).toString();

      const QString originalFilename = QgsProject::instance()->fileName();
      QString templateName = QFileInfo( originalFilename ).baseName();

      if ( templateName.isEmpty() )
      {
        bool ok;
        templateName = QInputDialog::getText( this, tr( "Template Name" ),
                                              tr( "Name for the template" ), QLineEdit::Normal,
                                              QString(), &ok );

        if ( !ok )
          return;
        if ( templateName.isEmpty() )
        {
          messageBar()->pushInfo( tr( "Template not saved" ), tr( "The template can not have an empty name." ) );
        }
      }
      const QString filePath = templateDirName + QDir::separator() + templateName + QStringLiteral( ".qgz" );
      if ( QFileInfo::exists( filePath ) )
      {
        QMessageBox msgBox( this );
        msgBox.setWindowTitle( tr( "Overwrite Template" ) );
        msgBox.setText( tr( "The template %1 already exists, do you want to replace it?" ).arg( templateName ) );
        msgBox.addButton( tr( "Overwrite" ), QMessageBox::YesRole );
        auto cancelButton = msgBox.addButton( QMessageBox::Cancel );
        msgBox.setIcon( QMessageBox::Question );
        msgBox.exec();
        if ( msgBox.clickedButton() == cancelButton )
        {
          return;
        }
      }

      QgsProject::instance()->write( filePath );
      QgsProject::instance()->setFileName( originalFilename );
      messageBar()->pushInfo( tr( "Template saved" ), tr( "Template %1 was saved" ).arg( templateName ) );

    } );
  }

  const QList<QgsProjectStorageGuiProvider *> storageGuiProviders = QgsGui::projectStorageGuiRegistry()->projectStorages();
  for ( QgsProjectStorageGuiProvider *storageGuiProvider : storageGuiProviders )
  {
    QString name = storageGuiProvider->visibleName();
    if ( name.isEmpty() )
      continue;
    QAction *action = menu->addAction( name + QChar( 0x2026 ) ); // 0x2026 = ellipsis character
    if ( saving )
    {
      connect( action, &QAction::triggered, this, [this, storageGuiProvider]
      {
        QString uri = storageGuiProvider->showSaveGui();
        if ( !uri.isEmpty() )
        {
          saveProjectToProjectStorage( uri );
        }
      } );
    }
    else
    {
      connect( action, &QAction::triggered, this, [this, storageGuiProvider]
      {
        QString uri = storageGuiProvider->showLoadGui();
        if ( !uri.isEmpty() )
        {
          addProject( uri );
        }
      } );
    }
  }

  // support legacy API (before 3.10 core and gui related functions were mixed together in QgsProjectStorage)
  const QList<QgsProjectStorage *> storages = QgsApplication::projectStorageRegistry()->projectStorages();
  for ( QgsProjectStorage *storage : storages )
  {
    Q_NOWARN_DEPRECATED_PUSH
    QString name = storage->visibleName();
    Q_NOWARN_DEPRECATED_POP
    if ( name.isEmpty() )
      continue;
    QAction *action = menu->addAction( name + QChar( 0x2026 ) ); // 0x2026 = ellipsis character
    if ( saving )
    {
      connect( action, &QAction::triggered, this, [this, storage]
      {
        Q_NOWARN_DEPRECATED_PUSH
        QString uri = storage->showSaveGui();
        Q_NOWARN_DEPRECATED_POP
        if ( !uri.isEmpty() )
          saveProjectToProjectStorage( uri );
      } );
    }
    else
    {
      connect( action, &QAction::triggered, this, [this, storage]
      {
        Q_NOWARN_DEPRECATED_PUSH
        QString uri = storage->showLoadGui();
        Q_NOWARN_DEPRECATED_POP
        if ( !uri.isEmpty() )
          addProject( uri );
      } );
    }
  }
}

void QgisApp::saveProjectToProjectStorage( const QString &uri )
{
  QgsProject::instance()->setFileName( uri );
  if ( QgsProject::instance()->write() )
  {
    setTitleBarText_( *this ); // update title bar
    mStatusBar->showMessage( tr( "Saved project to: %1" ).arg( uri ), 5000 );
    // add this to the list of recently used project files
    saveRecentProjectPath();
    mProjectLastModified = QgsProject::instance()->lastModified();
  }
  else
  {
    QMessageBox msgbox;

    msgbox.setWindowTitle( tr( "Save Project" ) );
    msgbox.setText( QgsProject::instance()->error() );
    msgbox.setIcon( QMessageBox::Icon::Critical );
    msgbox.addButton( QMessageBox::Cancel );
    msgbox.addButton( QMessageBox::Save );
    msgbox.setButtonText( QMessageBox::Save, tr( "Save as Local File" ) );
    msgbox.setDefaultButton( QMessageBox::Cancel );
    msgbox.exec();

    if ( msgbox.result() == QMessageBox::Save )
    {
      fileSaveAs();
    }
  }
}

#ifdef HAVE_CRASH_HANDLER
void QgisApp::triggerCrashHandler()
{
#ifdef Q_OS_WIN
  RaiseException( 0x12345678, 0, 0, nullptr );
#endif
}
#endif

void QgisApp::addTabifiedDockWidget( Qt::DockWidgetArea area, QDockWidget *dockWidget, const QStringList &tabifyWith, bool raiseTab )
{
  QList< QDockWidget * > dockWidgetsInArea;
  const auto dockWidgets = findChildren< QDockWidget * >();
  for ( QDockWidget *w : dockWidgets )
  {
    if ( w->isVisible() && dockWidgetArea( w ) == area )
    {
      dockWidgetsInArea << w;
    }
  }

  addDockWidget( area, dockWidget );  // First add the dock widget, then attempt to tabify
  if ( dockWidgetsInArea.length() > 0 )
  {
    // Get the base dock widget that we'll use to tabify our new dockWidget
    QDockWidget *tabifyWithDockWidget = nullptr;
    if ( !tabifyWith.isEmpty() )
    {
      // Iterate the list of object names looking for a
      // dock widget to tabify the new one on top of it
      bool objectNameFound = false;
      for ( int i = 0; i < tabifyWith.size(); i++ )
      {
        for ( QDockWidget *cw : dockWidgetsInArea )
        {
          if ( cw->objectName() == tabifyWith.at( i ) || cw->property( "dock_uuid" ).toString() == tabifyWith.at( i ) )
          {
            tabifyWithDockWidget = cw;
            objectNameFound = true;  // Also exit the outer for loop
            break;
          }
        }
        if ( objectNameFound )
        {
          break;
        }
      }
    }
    if ( !tabifyWithDockWidget )
    {
      tabifyWithDockWidget = dockWidgetsInArea.at( 0 );  // Last resort
    }
    if ( tabifyWithDockWidget == dockWidget )
      return;

    QTabBar *existentTabBar = nullptr;
    int currentIndex = -1;
    if ( !raiseTab && dockWidgetsInArea.length() > 1 )
    {
      // Chances are we've already got a tabBar, if so, get
      // currentIndex to restore status after inserting our new tab
      const QList<QTabBar *> tabBars = findChildren<QTabBar *>( QString(), Qt::FindDirectChildrenOnly );
      bool tabBarFound = false;
      for ( QTabBar *tabBar : tabBars )
      {
        for ( int i = 0; i < tabBar->count(); i++ )
        {
          if ( tabBar->tabText( i ) == tabifyWithDockWidget->windowTitle() )
          {
            existentTabBar = tabBar;
            currentIndex = tabBar->currentIndex();
            tabBarFound = true;
            break;
          }
        }
        if ( tabBarFound )
        {
          break;
        }
      }
    }

    // Now we can put the new dockWidget on top of tabifyWith
    tabifyDockWidget( tabifyWithDockWidget, dockWidget );

    // Should we restore dock widgets status?
    if ( !raiseTab )
    {
      if ( existentTabBar )
      {
        existentTabBar->setCurrentIndex( currentIndex );
      }
      else
      {
        tabifyWithDockWidget->raise();  // Single base dock widget, we can just raise it
      }
    }
  }
}

QgsAttributeEditorContext QgisApp::createAttributeEditorContext()
{
  QgsAttributeEditorContext context;
  context.setVectorLayerTools( vectorLayerTools() );
  context.setMapCanvas( mapCanvas() );
  context.setCadDockWidget( cadDockWidget() );
  context.setMainMessageBar( messageBar() );
  return context;
}

void QgisApp::showEvent( QShowEvent *event )
{
  QMainWindow::showEvent( event );
  // because of Qt regression: https://bugreports.qt.io/browse/QTBUG-89034
  // we have to wait till dialog is first shown to try to restore dock geometry or it's not correctly restored
  static std::once_flag firstShow;
  std::call_once( firstShow, [this]
  {
    QgsSettings settings;
    if ( !restoreState( settings.value( QStringLiteral( "UI/state" ), QByteArray::fromRawData( reinterpret_cast< const char * >( defaultUIstate ), sizeof defaultUIstate ) ).toByteArray() ) )
    {
      QgsDebugMsg( QStringLiteral( "restore of UI state failed" ) );
    }
  } );
}


void QgisApp::setupActions()
{
    mActionNewProject = new QAction(this);
    mActionNewProject->setObjectName(QString::fromUtf8("mActionNewProject"));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/themes/default/mActionFileNew.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewProject->setIcon(icon);
    mActionOpenProject = new QAction(this);
    mActionOpenProject->setObjectName(QString::fromUtf8("mActionOpenProject"));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/images/themes/default/mActionFileOpen.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOpenProject->setIcon(icon1);
    mActionSaveProject = new QAction(this);
    mActionSaveProject->setObjectName(QString::fromUtf8("mActionSaveProject"));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/images/themes/default/mActionFileSave.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSaveProject->setIcon(icon2);
    mActionSaveProjectAs = new QAction(this);
    mActionSaveProjectAs->setObjectName(QString::fromUtf8("mActionSaveProjectAs"));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/images/themes/default/mActionFileSaveAs.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSaveProjectAs->setIcon(icon3);
    mActionSaveMapAsImage = new QAction(this);
    mActionSaveMapAsImage->setObjectName(QString::fromUtf8("mActionSaveMapAsImage"));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/images/themes/default/mActionSaveMapAsImage.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSaveMapAsImage->setIcon(icon4);
    mActionSaveMapAsPdf = new QAction(this);
    mActionSaveMapAsPdf->setObjectName(QString::fromUtf8("mActionSaveMapAsPdf"));
    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/images/themes/default/mActionSaveAsPDF.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSaveMapAsPdf->setIcon(icon5);
    mActionNewMapCanvas = new QAction(this);
    mActionNewMapCanvas->setObjectName(QString::fromUtf8("mActionNewMapCanvas"));
    QIcon icon6;
    icon6.addFile(QString::fromUtf8(":/images/themes/default/mActionNewMap.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewMapCanvas->setIcon(icon6);
    mActionExit = new QAction(this);
    mActionExit->setObjectName(QString::fromUtf8("mActionExit"));
    QIcon icon7;
    icon7.addFile(QString::fromUtf8(":/images/themes/default/mActionFileExit.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionExit->setIcon(icon7);
    mActionExit->setMenuRole(QAction::QuitRole);
    mActionUndo = new QAction(this);
    mActionUndo->setObjectName(QString::fromUtf8("mActionUndo"));
    QIcon icon8;
    icon8.addFile(QString::fromUtf8(":/images/themes/default/mActionUndo.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionUndo->setIcon(icon8);
    mActionRedo = new QAction(this);
    mActionRedo->setObjectName(QString::fromUtf8("mActionRedo"));
    QIcon icon9;
    icon9.addFile(QString::fromUtf8(":/images/themes/default/mActionRedo.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionRedo->setIcon(icon9);
    mActionCutFeatures = new QAction(this);
    mActionCutFeatures->setObjectName(QString::fromUtf8("mActionCutFeatures"));
    QIcon icon10;
    icon10.addFile(QString::fromUtf8(":/images/themes/default/mActionEditCut.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionCutFeatures->setIcon(icon10);
    mActionCopyFeatures = new QAction(this);
    mActionCopyFeatures->setObjectName(QString::fromUtf8("mActionCopyFeatures"));
    QIcon icon11;
    icon11.addFile(QString::fromUtf8(":/images/themes/default/mActionEditCopy.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionCopyFeatures->setIcon(icon11);
    mActionPasteFeatures = new QAction(this);
    mActionPasteFeatures->setObjectName(QString::fromUtf8("mActionPasteFeatures"));
    QIcon icon12;
    icon12.addFile(QString::fromUtf8(":/images/themes/default/mActionEditPaste.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionPasteFeatures->setIcon(icon12);
    mActionAddFeature = new QAction(this);
    mActionAddFeature->setObjectName(QString::fromUtf8("mActionAddFeature"));
    mActionAddFeature->setCheckable(true);
    QIcon icon13;
    icon13.addFile(QString::fromUtf8(":/images/themes/default/mActionCapturePolygon.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddFeature->setIcon(icon13);
    mActionMoveFeature = new QAction(this);
    mActionMoveFeature->setObjectName(QString::fromUtf8("mActionMoveFeature"));
    mActionMoveFeature->setCheckable(true);
    QIcon icon14;
    icon14.addFile(QString::fromUtf8(":/images/themes/default/mActionMoveFeature.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMoveFeature->setIcon(icon14);
    mActionReshapeFeatures = new QAction(this);
    mActionReshapeFeatures->setObjectName(QString::fromUtf8("mActionReshapeFeatures"));
    mActionReshapeFeatures->setCheckable(true);
    QIcon icon15;
    icon15.addFile(QString::fromUtf8(":/images/themes/default/mActionReshape.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionReshapeFeatures->setIcon(icon15);
    mActionSplitFeatures = new QAction(this);
    mActionSplitFeatures->setObjectName(QString::fromUtf8("mActionSplitFeatures"));
    mActionSplitFeatures->setCheckable(true);
    QIcon icon16;
    icon16.addFile(QString::fromUtf8(":/images/themes/default/mActionSplitFeatures.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSplitFeatures->setIcon(icon16);
    mActionSplitParts = new QAction(this);
    mActionSplitParts->setObjectName(QString::fromUtf8("mActionSplitParts"));
    mActionSplitParts->setCheckable(true);
    mActionSplitParts->setIcon(icon16);
    mActionDeleteSelected = new QAction(this);
    mActionDeleteSelected->setObjectName(QString::fromUtf8("mActionDeleteSelected"));
    QIcon icon17;
    icon17.addFile(QString::fromUtf8(":/images/themes/default/mActionDeleteSelectedFeatures.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDeleteSelected->setIcon(icon17);
    mActionAddRing = new QAction(this);
    mActionAddRing->setObjectName(QString::fromUtf8("mActionAddRing"));
    mActionAddRing->setCheckable(true);
    QIcon icon18;
    icon18.addFile(QString::fromUtf8(":/images/themes/default/mActionAddRing.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddRing->setIcon(icon18);
    mActionAddPart = new QAction(this);
    mActionAddPart->setObjectName(QString::fromUtf8("mActionAddPart"));
    mActionAddPart->setCheckable(true);
    QIcon icon19;
    icon19.addFile(QString::fromUtf8(":/images/themes/default/mActionAddPart.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddPart->setIcon(icon19);
    mActionSimplifyFeature = new QAction(this);
    mActionSimplifyFeature->setObjectName(QString::fromUtf8("mActionSimplifyFeature"));
    mActionSimplifyFeature->setCheckable(true);
    QIcon icon20;
    icon20.addFile(QString::fromUtf8(":/images/themes/default/mActionSimplify.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSimplifyFeature->setIcon(icon20);
    mActionDeleteRing = new QAction(this);
    mActionDeleteRing->setObjectName(QString::fromUtf8("mActionDeleteRing"));
    mActionDeleteRing->setCheckable(true);
    QIcon icon21;
    icon21.addFile(QString::fromUtf8(":/images/themes/default/mActionDeleteRing.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDeleteRing->setIcon(icon21);
    mActionDeletePart = new QAction(this);
    mActionDeletePart->setObjectName(QString::fromUtf8("mActionDeletePart"));
    mActionDeletePart->setCheckable(true);
    QIcon icon22;
    icon22.addFile(QString::fromUtf8(":/images/themes/default/mActionDeletePart.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDeletePart->setIcon(icon22);
    mActionMergeFeatures = new QAction(this);
    mActionMergeFeatures->setObjectName(QString::fromUtf8("mActionMergeFeatures"));
    QIcon icon23;
    icon23.addFile(QString::fromUtf8(":/images/themes/default/mActionMergeFeatures.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMergeFeatures->setIcon(icon23);
    mActionMergeFeatureAttributes = new QAction(this);
    mActionMergeFeatureAttributes->setObjectName(QString::fromUtf8("mActionMergeFeatureAttributes"));
    QIcon icon24;
    icon24.addFile(QString::fromUtf8(":/images/themes/default/mActionMergeFeatureAttributes.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMergeFeatureAttributes->setIcon(icon24);
    mActionVertexTool = new QAction(this);
    mActionVertexTool->setObjectName(QString::fromUtf8("mActionVertexTool"));
    mActionVertexTool->setCheckable(true);
    QIcon icon25;
    icon25.addFile(QString::fromUtf8(":/images/themes/default/mActionVertexTool.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionVertexTool->setIcon(icon25);
    mActionRotatePointSymbols = new QAction(this);
    mActionRotatePointSymbols->setObjectName(QString::fromUtf8("mActionRotatePointSymbols"));
    mActionRotatePointSymbols->setCheckable(true);
    QIcon icon26;
    icon26.addFile(QString::fromUtf8(":/images/themes/default/mActionRotatePointSymbols.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionRotatePointSymbols->setIcon(icon26);
    mActionOffsetPointSymbol = new QAction(this);
    mActionOffsetPointSymbol->setObjectName(QString::fromUtf8("mActionOffsetPointSymbol"));
    mActionOffsetPointSymbol->setCheckable(true);
    QIcon icon27;
    icon27.addFile(QString::fromUtf8(":/images/themes/default/mActionMoveVertex.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOffsetPointSymbol->setIcon(icon27);
    mActionReverseLine = new QAction(this);
    mActionReverseLine->setObjectName(QString::fromUtf8("mActionReverseLine"));
    mActionReverseLine->setCheckable(true);
    QIcon icon28;
    icon28.addFile(QString::fromUtf8(":/images/themes/default/mActionReverseLine.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionReverseLine->setIcon(icon28);
    mActionTrimExtendFeature = new QAction(this);
    mActionTrimExtendFeature->setObjectName(QString::fromUtf8("mActionTrimExtendFeature"));
    mActionTrimExtendFeature->setCheckable(true);
    QIcon icon29;
    icon29.addFile(QString::fromUtf8(":/images/themes/default/mActionTrimExtendFeature.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionTrimExtendFeature->setIcon(icon29);
    mActionSnappingOptions = new QAction(this);
    mActionSnappingOptions->setObjectName(QString::fromUtf8("mActionSnappingOptions"));
    mActionPan = new QAction(this);
    mActionPan->setObjectName(QString::fromUtf8("mActionPan"));
    mActionPan->setCheckable(true);
    QIcon icon30;
    icon30.addFile(QString::fromUtf8(":/images/themes/default/mActionPan.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionPan->setIcon(icon30);
    mActionZoomIn = new QAction(this);
    mActionZoomIn->setObjectName(QString::fromUtf8("mActionZoomIn"));
    mActionZoomIn->setCheckable(true);
    QIcon icon31;
    icon31.addFile(QString::fromUtf8(":/images/themes/default/mActionZoomIn.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionZoomIn->setIcon(icon31);
    mActionZoomOut = new QAction(this);
    mActionZoomOut->setObjectName(QString::fromUtf8("mActionZoomOut"));
    mActionZoomOut->setCheckable(true);
    QIcon icon32;
    icon32.addFile(QString::fromUtf8(":/images/themes/default/mActionZoomOut.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionZoomOut->setIcon(icon32);
    mActionSelectFeatures = new QAction(this);
    mActionSelectFeatures->setObjectName(QString::fromUtf8("mActionSelectFeatures"));
    mActionSelectFeatures->setCheckable(true);
    QIcon icon33;
    icon33.addFile(QString::fromUtf8(":/images/themes/default/mActionSelectRectangle.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSelectFeatures->setIcon(icon33);
    mActionSelectPolygon = new QAction(this);
    mActionSelectPolygon->setObjectName(QString::fromUtf8("mActionSelectPolygon"));
    mActionSelectPolygon->setCheckable(true);
    QIcon icon34;
    icon34.addFile(QString::fromUtf8(":/images/themes/default/mActionSelectPolygon.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSelectPolygon->setIcon(icon34);
    mActionSelectFreehand = new QAction(this);
    mActionSelectFreehand->setObjectName(QString::fromUtf8("mActionSelectFreehand"));
    mActionSelectFreehand->setCheckable(true);
    QIcon icon35;
    icon35.addFile(QString::fromUtf8(":/images/themes/default/mActionSelectFreehand.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSelectFreehand->setIcon(icon35);
    mActionSelectRadius = new QAction(this);
    mActionSelectRadius->setObjectName(QString::fromUtf8("mActionSelectRadius"));
    mActionSelectRadius->setCheckable(true);
    QIcon icon36;
    icon36.addFile(QString::fromUtf8(":/images/themes/default/mActionSelectRadius.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSelectRadius->setIcon(icon36);
    mActionDeselectAll = new QAction(this);
    mActionDeselectAll->setObjectName(QString::fromUtf8("mActionDeselectAll"));
    QIcon icon37;
    icon37.addFile(QString::fromUtf8(":/images/themes/default/mActionDeselectAll.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDeselectAll->setIcon(icon37);
    mActionDeselectActiveLayer = new QAction(this);
    mActionDeselectActiveLayer->setObjectName(QString::fromUtf8("mActionDeselectActiveLayer"));
    QIcon icon38;
    icon38.addFile(QString::fromUtf8(":/images/themes/default/mActionDeselectActiveLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDeselectActiveLayer->setIcon(icon38);
    mActionSelectAll = new QAction(this);
    mActionSelectAll->setObjectName(QString::fromUtf8("mActionSelectAll"));
    QIcon icon39;
    icon39.addFile(QString::fromUtf8(":/images/themes/default/mActionSelectAll.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSelectAll->setIcon(icon39);
    mActionInvertSelection = new QAction(this);
    mActionInvertSelection->setObjectName(QString::fromUtf8("mActionInvertSelection"));
    QIcon icon40;
    icon40.addFile(QString::fromUtf8(":/images/themes/default/mActionInvertSelection.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionInvertSelection->setIcon(icon40);
    mActionIdentify = new QAction(this);
    mActionIdentify->setObjectName(QString::fromUtf8("mActionIdentify"));
    mActionIdentify->setCheckable(true);
    QIcon icon41;
    icon41.addFile(QString::fromUtf8(":/images/themes/default/mActionIdentify.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionIdentify->setIcon(icon41);
    mActionMeasure = new QAction(this);
    mActionMeasure->setObjectName(QString::fromUtf8("mActionMeasure"));
    mActionMeasure->setCheckable(true);
    QIcon icon42;
    icon42.addFile(QString::fromUtf8(":/images/themes/default/mActionMeasure.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMeasure->setIcon(icon42);
    mActionMeasureArea = new QAction(this);
    mActionMeasureArea->setObjectName(QString::fromUtf8("mActionMeasureArea"));
    mActionMeasureArea->setCheckable(true);
    QIcon icon43;
    icon43.addFile(QString::fromUtf8(":/images/themes/default/mActionMeasureArea.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMeasureArea->setIcon(icon43);
    mActionMeasureAngle = new QAction(this);
    mActionMeasureAngle->setObjectName(QString::fromUtf8("mActionMeasureAngle"));
    mActionMeasureAngle->setCheckable(true);
    QIcon icon44;
    icon44.addFile(QString::fromUtf8(":/images/themes/default/mActionMeasureAngle.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMeasureAngle->setIcon(icon44);
    mActionZoomFullExtent = new QAction(this);
    mActionZoomFullExtent->setObjectName(QString::fromUtf8("mActionZoomFullExtent"));
    QIcon icon45;
    icon45.addFile(QString::fromUtf8(":/images/themes/default/mActionZoomFullExtent.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionZoomFullExtent->setIcon(icon45);
    mActionZoomToLayer = new QAction(this);
    mActionZoomToLayer->setObjectName(QString::fromUtf8("mActionZoomToLayer"));
    QIcon icon46;
    icon46.addFile(QString::fromUtf8(":/images/themes/default/mActionZoomToLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionZoomToLayer->setIcon(icon46);
    mActionZoomToLayers = new QAction(this);
    mActionZoomToLayers->setObjectName(QString::fromUtf8("mActionZoomToLayers"));
    mActionZoomToLayers->setIcon(icon46);
    mActionZoomToSelected = new QAction(this);
    mActionZoomToSelected->setObjectName(QString::fromUtf8("mActionZoomToSelected"));
    QIcon icon47;
    icon47.addFile(QString::fromUtf8(":/images/themes/default/mActionZoomToSelected.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionZoomToSelected->setIcon(icon47);
    mActionZoomLast = new QAction(this);
    mActionZoomLast->setObjectName(QString::fromUtf8("mActionZoomLast"));
    QIcon icon48;
    icon48.addFile(QString::fromUtf8(":/images/themes/default/mActionZoomLast.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionZoomLast->setIcon(icon48);
    mActionZoomNext = new QAction(this);
    mActionZoomNext->setObjectName(QString::fromUtf8("mActionZoomNext"));
    QIcon icon49;
    icon49.addFile(QString::fromUtf8(":/images/themes/default/mActionZoomNext.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionZoomNext->setIcon(icon49);
    mActionZoomActualSize = new QAction(this);
    mActionZoomActualSize->setObjectName(QString::fromUtf8("mActionZoomActualSize"));
    QIcon icon50;
    icon50.addFile(QString::fromUtf8(":/images/themes/default/mActionZoomActual.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionZoomActualSize->setIcon(icon50);
    mActionMapTips = new QAction(this);
    mActionMapTips->setObjectName(QString::fromUtf8("mActionMapTips"));
    mActionMapTips->setCheckable(true);
    QIcon icon51;
    icon51.addFile(QString::fromUtf8(":/images/themes/default/mActionMapTips.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMapTips->setIcon(icon51);
    mActionNewBookmark = new QAction(this);
    mActionNewBookmark->setObjectName(QString::fromUtf8("mActionNewBookmark"));
    QIcon icon52;
    icon52.addFile(QString::fromUtf8(":/images/themes/default/mActionNewBookmark.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewBookmark->setIcon(icon52);
    mActionShowBookmarks = new QAction(this);
    mActionShowBookmarks->setObjectName(QString::fromUtf8("mActionShowBookmarks"));
    QIcon icon53;
    icon53.addFile(QString::fromUtf8(":/images/themes/default/mActionShowBookmarks.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowBookmarks->setIcon(icon53);
    mActionShowBookmarkManager = new QAction(this);
    mActionShowBookmarkManager->setObjectName(QString::fromUtf8("mActionShowBookmarkManager"));
    mActionShowBookmarkManager->setCheckable(true);
    mActionShowBookmarkManager->setIcon(icon53);
    mActionDraw = new QAction(this);
    mActionDraw->setObjectName(QString::fromUtf8("mActionDraw"));
    QIcon icon54;
    icon54.addFile(QString::fromUtf8(":/images/themes/default/mActionRefresh.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDraw->setIcon(icon54);
    mActionTextAnnotation = new QAction(this);
    mActionTextAnnotation->setObjectName(QString::fromUtf8("mActionTextAnnotation"));
    mActionTextAnnotation->setCheckable(true);
    QIcon icon55;
    icon55.addFile(QString::fromUtf8(":/images/themes/default/mActionTextAnnotation.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionTextAnnotation->setIcon(icon55);
    mActionFormAnnotation = new QAction(this);
    mActionFormAnnotation->setObjectName(QString::fromUtf8("mActionFormAnnotation"));
    mActionFormAnnotation->setCheckable(true);
    QIcon icon56;
    icon56.addFile(QString::fromUtf8(":/images/themes/default/mActionFormAnnotation.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionFormAnnotation->setIcon(icon56);
    mActionAnnotation = new QAction(this);
    mActionAnnotation->setObjectName(QString::fromUtf8("mActionAnnotation"));
    mActionAnnotation->setCheckable(true);
    QIcon icon57;
    icon57.addFile(QString::fromUtf8(":/images/themes/default/mActionAnnotation.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAnnotation->setIcon(icon57);
    mActionLabeling = new QAction(this);
    mActionLabeling->setObjectName(QString::fromUtf8("mActionLabeling"));
    QIcon icon58;
    icon58.addFile(QString::fromUtf8(":/images/themes/default/mActionLabeling.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionLabeling->setIcon(icon58);
    mActionNewVectorLayer = new QAction(this);
    mActionNewVectorLayer->setObjectName(QString::fromUtf8("mActionNewVectorLayer"));
    QIcon icon59;
    icon59.addFile(QString::fromUtf8(":/images/themes/default/mActionNewVectorLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewVectorLayer->setIcon(icon59);
    mActionNewSpatiaLiteLayer = new QAction(this);
    mActionNewSpatiaLiteLayer->setObjectName(QString::fromUtf8("mActionNewSpatiaLiteLayer"));
    QIcon icon60;
    icon60.addFile(QString::fromUtf8(":/images/themes/default/mActionNewSpatiaLiteLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewSpatiaLiteLayer->setIcon(icon60);
    mActionShowRasterCalculator = new QAction(this);
    mActionShowRasterCalculator->setObjectName(QString::fromUtf8("mActionShowRasterCalculator"));
    QIcon icon61;
    icon61.addFile(QString::fromUtf8(":/images/themes/default/mActionShowRasterCalculator.png"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowRasterCalculator->setIcon(icon61);
    mActionAddOgrLayer = new QAction(this);
    mActionAddOgrLayer->setObjectName(QString::fromUtf8("mActionAddOgrLayer"));
    QIcon icon62;
    icon62.addFile(QString::fromUtf8(":/images/themes/default/mActionAddOgrLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddOgrLayer->setIcon(icon62);
    mActionAddRasterLayer = new QAction(this);
    mActionAddRasterLayer->setObjectName(QString::fromUtf8("mActionAddRasterLayer"));
    QIcon icon63;
    icon63.addFile(QString::fromUtf8(":/images/themes/default/mActionAddRasterLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddRasterLayer->setIcon(icon63);
    mActionAddPgLayer = new QAction(this);
    mActionAddPgLayer->setObjectName(QString::fromUtf8("mActionAddPgLayer"));
    QIcon icon64;
    icon64.addFile(QString::fromUtf8(":/images/themes/default/mActionAddPostgisLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddPgLayer->setIcon(icon64);
    mActionAddSpatiaLiteLayer = new QAction(this);
    mActionAddSpatiaLiteLayer->setObjectName(QString::fromUtf8("mActionAddSpatiaLiteLayer"));
    QIcon icon65;
    icon65.addFile(QString::fromUtf8(":/images/themes/default/mActionAddSpatiaLiteLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddSpatiaLiteLayer->setIcon(icon65);
    mActionAddMssqlLayer = new QAction(this);
    mActionAddMssqlLayer->setObjectName(QString::fromUtf8("mActionAddMssqlLayer"));
    QIcon icon66;
    icon66.addFile(QString::fromUtf8(":/images/themes/default/mActionAddMssqlLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddMssqlLayer->setIcon(icon66);
    mActionAddOracleLayer = new QAction(this);
    mActionAddOracleLayer->setObjectName(QString::fromUtf8("mActionAddOracleLayer"));
    QIcon icon67;
    icon67.addFile(QString::fromUtf8(":/images/themes/default/mActionAddOracleLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddOracleLayer->setIcon(icon67);
    mActionAddHanaLayer = new QAction(this);
    mActionAddHanaLayer->setObjectName(QString::fromUtf8("mActionAddHanaLayer"));
    QIcon icon68;
    icon68.addFile(QString::fromUtf8(":/images/themes/default/mActionAddHanaLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddHanaLayer->setIcon(icon68);
    mActionAddWmsLayer = new QAction(this);
    mActionAddWmsLayer->setObjectName(QString::fromUtf8("mActionAddWmsLayer"));
    QIcon icon69;
    icon69.addFile(QString::fromUtf8(":/images/themes/default/mActionAddWmsLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddWmsLayer->setIcon(icon69);
    mActionOpenTable = new QAction(this);
    mActionOpenTable->setObjectName(QString::fromUtf8("mActionOpenTable"));
    QIcon icon70;
    icon70.addFile(QString::fromUtf8(":/images/themes/default/mActionOpenTable.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOpenTable->setIcon(icon70);
    mActionOpenTableSelected = new QAction(this);
    mActionOpenTableSelected->setObjectName(QString::fromUtf8("mActionOpenTableSelected"));
    QIcon icon71;
    icon71.addFile(QString::fromUtf8(":/images/themes/default/mActionOpenTableSelected.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOpenTableSelected->setIcon(icon71);
    mActionOpenTableVisible = new QAction(this);
    mActionOpenTableVisible->setObjectName(QString::fromUtf8("mActionOpenTableVisible"));
    QIcon icon72;
    icon72.addFile(QString::fromUtf8(":/images/themes/default/mActionOpenTableVisible.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOpenTableVisible->setIcon(icon72);
    mActionOpenTableEdited = new QAction(this);
    mActionOpenTableEdited->setObjectName(QString::fromUtf8("mActionOpenTableEdited"));
    QIcon icon73;
    icon73.addFile(QString::fromUtf8(":/images/themes/default/mActionOpenTableEdited.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOpenTableEdited->setIcon(icon73);
    mActionToggleEditing = new QAction(this);
    mActionToggleEditing->setObjectName(QString::fromUtf8("mActionToggleEditing"));
    mActionToggleEditing->setCheckable(true);
    QIcon icon74;
    icon74.addFile(QString::fromUtf8(":/images/themes/default/mActionToggleEditing.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionToggleEditing->setIcon(icon74);
    mActionSaveEdits = new QAction(this);
    mActionSaveEdits->setObjectName(QString::fromUtf8("mActionSaveEdits"));
    QIcon icon75;
    icon75.addFile(QString::fromUtf8(":/images/themes/default/mActionSaveEdits.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSaveEdits->setIcon(icon75);
    mActionLayerSaveAs = new QAction(this);
    mActionLayerSaveAs->setObjectName(QString::fromUtf8("mActionLayerSaveAs"));
    mActionRemoveLayer = new QAction(this);
    mActionRemoveLayer->setObjectName(QString::fromUtf8("mActionRemoveLayer"));
    QIcon icon76;
    icon76.addFile(QString::fromUtf8(":/images/themes/default/mActionRemoveLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionRemoveLayer->setIcon(icon76);
    mActionSetLayerCRS = new QAction(this);
    mActionSetLayerCRS->setObjectName(QString::fromUtf8("mActionSetLayerCRS"));
    mActionSetProjectCRSFromLayer = new QAction(this);
    mActionSetProjectCRSFromLayer->setObjectName(QString::fromUtf8("mActionSetProjectCRSFromLayer"));
    mActionLayerProperties = new QAction(this);
    mActionLayerProperties->setObjectName(QString::fromUtf8("mActionLayerProperties"));
    mActionLayerSubsetString = new QAction(this);
    mActionLayerSubsetString->setObjectName(QString::fromUtf8("mActionLayerSubsetString"));
    mActionAddToOverview = new QAction(this);
    mActionAddToOverview->setObjectName(QString::fromUtf8("mActionAddToOverview"));
    QIcon icon77;
    icon77.addFile(QString::fromUtf8(":/images/themes/default/mActionInOverview.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddToOverview->setIcon(icon77);
    mActionAddAllToOverview = new QAction(this);
    mActionAddAllToOverview->setObjectName(QString::fromUtf8("mActionAddAllToOverview"));
    QIcon icon78;
    icon78.addFile(QString::fromUtf8(":/images/themes/default/mActionAddAllToOverview.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddAllToOverview->setIcon(icon78);
    mActionRemoveAllFromOverview = new QAction(this);
    mActionRemoveAllFromOverview->setObjectName(QString::fromUtf8("mActionRemoveAllFromOverview"));
    QIcon icon79;
    icon79.addFile(QString::fromUtf8(":/images/themes/default/mActionRemoveAllFromOverview.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionRemoveAllFromOverview->setIcon(icon79);
    mActionShowAllLayers = new QAction(this);
    mActionShowAllLayers->setObjectName(QString::fromUtf8("mActionShowAllLayers"));
    QIcon icon80;
    icon80.addFile(QString::fromUtf8(":/images/themes/default/mActionShowAllLayers.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowAllLayers->setIcon(icon80);
    mActionHideAllLayers = new QAction(this);
    mActionHideAllLayers->setObjectName(QString::fromUtf8("mActionHideAllLayers"));
    QIcon icon81;
    icon81.addFile(QString::fromUtf8(":/images/themes/default/mActionHideAllLayers.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionHideAllLayers->setIcon(icon81);
    mActionManagePlugins = new QAction(this);
    mActionManagePlugins->setObjectName(QString::fromUtf8("mActionManagePlugins"));
    QIcon icon82;
    icon82.addFile(QString::fromUtf8(":/images/themes/default/mActionShowPluginManager.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionManagePlugins->setIcon(icon82);
    mActionToggleFullScreen = new QAction(this);
    mActionToggleFullScreen->setObjectName(QString::fromUtf8("mActionToggleFullScreen"));
    mActionTogglePanelsVisibility = new QAction(this);
    mActionTogglePanelsVisibility->setObjectName(QString::fromUtf8("mActionTogglePanelsVisibility"));
    mActionToggleMapOnly = new QAction(this);
    mActionToggleMapOnly->setObjectName(QString::fromUtf8("mActionToggleMapOnly"));
    mActionProjectProperties = new QAction(this);
    mActionProjectProperties->setObjectName(QString::fromUtf8("mActionProjectProperties"));
    QIcon icon83;
    icon83.addFile(QString::fromUtf8(":/images/themes/default/mActionProjectProperties.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionProjectProperties->setIcon(icon83);
    mActionOptions = new QAction(this);
    mActionOptions->setObjectName(QString::fromUtf8("mActionOptions"));
    QIcon icon84;
    icon84.addFile(QString::fromUtf8(":/images/themes/default/mActionOptions.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOptions->setIcon(icon84);
    mActionOptions->setMenuRole(QAction::NoRole);
    mActionCustomProjection = new QAction(this);
    mActionCustomProjection->setObjectName(QString::fromUtf8("mActionCustomProjection"));
    QIcon icon85;
    icon85.addFile(QString::fromUtf8(":/images/themes/default/mActionCustomProjection.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionCustomProjection->setIcon(icon85);
    mActionConfigureShortcuts = new QAction(this);
    mActionConfigureShortcuts->setObjectName(QString::fromUtf8("mActionConfigureShortcuts"));
    QIcon icon86;
    icon86.addFile(QString::fromUtf8(":/images/themes/default/mActionKeyboardShortcuts.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionConfigureShortcuts->setIcon(icon86);
    mActionConfigureShortcuts->setMenuRole(QAction::NoRole);
    mActionLocalHistogramStretch = new QAction(this);
    mActionLocalHistogramStretch->setObjectName(QString::fromUtf8("mActionLocalHistogramStretch"));
    QIcon icon87;
    icon87.addFile(QString::fromUtf8(":/images/themes/default/mActionLocalHistogramStretch.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionLocalHistogramStretch->setIcon(icon87);
    mActionHelpContents = new QAction(this);
    mActionHelpContents->setObjectName(QString::fromUtf8("mActionHelpContents"));
    QIcon icon88;
    icon88.addFile(QString::fromUtf8(":/images/themes/default/mActionHelpContents.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionHelpContents->setIcon(icon88);
    mActionHelpAPI = new QAction(this);
    mActionHelpAPI->setObjectName(QString::fromUtf8("mActionHelpAPI"));
    mActionQgisHomePage = new QAction(this);
    mActionQgisHomePage->setObjectName(QString::fromUtf8("mActionQgisHomePage"));
    QIcon icon89;
    icon89.addFile(QString::fromUtf8(":/images/themes/default/mActionQgisHomePage.png"), QSize(), QIcon::Normal, QIcon::Off);
    mActionQgisHomePage->setIcon(icon89);
    mActionCheckQgisVersion = new QAction(this);
    mActionCheckQgisVersion->setObjectName(QString::fromUtf8("mActionCheckQgisVersion"));
    QIcon icon90;
    icon90.addFile(QString::fromUtf8(":/images/themes/default/mIconSuccess.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionCheckQgisVersion->setIcon(icon90);
    mActionAbout = new QAction(this);
    mActionAbout->setObjectName(QString::fromUtf8("mActionAbout"));
    QIcon icon91;
    icon91.addFile(QString::fromUtf8(":/images/themes/default/mActionHelpAbout.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAbout->setIcon(icon91);
    mActionAbout->setMenuRole(QAction::AboutRole);
    mActionSponsors = new QAction(this);
    mActionSponsors->setObjectName(QString::fromUtf8("mActionSponsors"));
    QIcon icon92;
    icon92.addFile(QString::fromUtf8(":/images/themes/default/mActionHelpSponsors.png"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSponsors->setIcon(icon92);
    mActionMoveLabel = new QAction(this);
    mActionMoveLabel->setObjectName(QString::fromUtf8("mActionMoveLabel"));
    mActionMoveLabel->setCheckable(true);
    QIcon icon93;
    icon93.addFile(QString::fromUtf8(":/images/themes/default/mActionMoveLabel.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMoveLabel->setIcon(icon93);
    mActionRotateLabel = new QAction(this);
    mActionRotateLabel->setObjectName(QString::fromUtf8("mActionRotateLabel"));
    mActionRotateLabel->setCheckable(true);
    QIcon icon94;
    icon94.addFile(QString::fromUtf8(":/images/themes/default/mActionRotateLabel.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionRotateLabel->setIcon(icon94);
    mActionChangeLabelProperties = new QAction(this);
    mActionChangeLabelProperties->setObjectName(QString::fromUtf8("mActionChangeLabelProperties"));
    mActionChangeLabelProperties->setCheckable(true);
    QIcon icon95;
    icon95.addFile(QString::fromUtf8(":/images/themes/default/mActionChangeLabelProperties.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionChangeLabelProperties->setIcon(icon95);
    mActionStyleManager = new QAction(this);
    mActionStyleManager->setObjectName(QString::fromUtf8("mActionStyleManager"));
    QIcon icon96;
    icon96.addFile(QString::fromUtf8(":/images/themes/default/mActionStyleManager.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionStyleManager->setIcon(icon96);
    mActionShowPythonDialog = new QAction(this);
    mActionShowPythonDialog->setObjectName(QString::fromUtf8("mActionShowPythonDialog"));
    mActionShowPythonDialog->setCheckable(true);
    QIcon icon97;
    icon97.addFile(QString::fromUtf8(":/images/themes/default/console/mIconRunConsole.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowPythonDialog->setIcon(icon97);
    mActionFullHistogramStretch = new QAction(this);
    mActionFullHistogramStretch->setObjectName(QString::fromUtf8("mActionFullHistogramStretch"));
    QIcon icon98;
    icon98.addFile(QString::fromUtf8(":/images/themes/default/mActionFullHistogramStretch.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionFullHistogramStretch->setIcon(icon98);
    mActionAddLayerSeparator = new QAction(this);
    mActionAddLayerSeparator->setObjectName(QString::fromUtf8("mActionAddLayerSeparator"));
    mActionAddLayerSeparator->setText(QString::fromUtf8("More Add Layer actions here"));
    mActionAddLayerSeparator->setIconText(QString::fromUtf8("More Add Layer actions here"));
#if QT_CONFIG(tooltip)
    mActionAddLayerSeparator->setToolTip(QString::fromUtf8("More Add Layer Actions here"));
#endif // QT_CONFIG(tooltip)
    mActionAddLayerSeparator->setVisible(false);
    mActionCustomization = new QAction(this);
    mActionCustomization->setObjectName(QString::fromUtf8("mActionCustomization"));
    QIcon icon99;
    icon99.addFile(QString::fromUtf8(":/images/themes/default/mActionInterfaceCustomization.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionCustomization->setIcon(icon99);
    actionActionCatchForCustomization = new QAction(this);
    actionActionCatchForCustomization->setObjectName(QString::fromUtf8("actionActionCatchForCustomization"));
    actionActionCatchForCustomization->setText(QString::fromUtf8("mActionCatchForCustomization"));
    mActionEmbedLayers = new QAction(this);
    mActionEmbedLayers->setObjectName(QString::fromUtf8("mActionEmbedLayers"));
    mActionDecorationTitle = new QAction(this);
    mActionDecorationTitle->setObjectName(QString::fromUtf8("mActionDecorationTitle"));
    QIcon icon100;
    icon100.addFile(QString::fromUtf8(":/images/themes/default/copyright_label.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecorationTitle->setIcon(icon100);
    mActionDecorationCopyright = new QAction(this);
    mActionDecorationCopyright->setObjectName(QString::fromUtf8("mActionDecorationCopyright"));
    mActionDecorationCopyright->setIcon(icon100);
    mActionDecorationImage = new QAction(this);
    mActionDecorationImage->setObjectName(QString::fromUtf8("mActionDecorationImage"));
    QIcon icon101;
    icon101.addFile(QString::fromUtf8(":/images/themes/default/mActionAddImage.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecorationImage->setIcon(icon101);
    mActionDecorationNorthArrow = new QAction(this);
    mActionDecorationNorthArrow->setObjectName(QString::fromUtf8("mActionDecorationNorthArrow"));
    QIcon icon102;
    icon102.addFile(QString::fromUtf8(":/images/themes/default/north_arrow.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecorationNorthArrow->setIcon(icon102);
    mActionDecorationScaleBar = new QAction(this);
    mActionDecorationScaleBar->setObjectName(QString::fromUtf8("mActionDecorationScaleBar"));
    QIcon icon103;
    icon103.addFile(QString::fromUtf8(":/images/themes/default/mActionScaleBar.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecorationScaleBar->setIcon(icon103);
    mActionAddWfsLayer = new QAction(this);
    mActionAddWfsLayer->setObjectName(QString::fromUtf8("mActionAddWfsLayer"));
    QIcon icon104;
    icon104.addFile(QString::fromUtf8(":/images/themes/default/mActionAddWfsLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddWfsLayer->setIcon(icon104);
    mActionFeatureAction = new QAction(this);
    mActionFeatureAction->setObjectName(QString::fromUtf8("mActionFeatureAction"));
    mActionFeatureAction->setCheckable(true);
    QIcon icon105;
    icon105.addFile(QString::fromUtf8(":/images/themes/default/mAction.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionFeatureAction->setIcon(icon105);
    mActionPanToSelected = new QAction(this);
    mActionPanToSelected->setObjectName(QString::fromUtf8("mActionPanToSelected"));
    QIcon icon106;
    icon106.addFile(QString::fromUtf8(":/images/themes/default/mActionPanToSelected.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionPanToSelected->setIcon(icon106);
    mActionOffsetCurve = new QAction(this);
    mActionOffsetCurve->setObjectName(QString::fromUtf8("mActionOffsetCurve"));
    mActionOffsetCurve->setCheckable(true);
    QIcon icon107;
    icon107.addFile(QString::fromUtf8(":/images/themes/default/mActionOffsetCurve.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOffsetCurve->setIcon(icon107);
    mActionCopyStyle = new QAction(this);
    mActionCopyStyle->setObjectName(QString::fromUtf8("mActionCopyStyle"));
    mActionCopyStyle->setIcon(icon11);
    mActionPasteStyle = new QAction(this);
    mActionPasteStyle->setObjectName(QString::fromUtf8("mActionPasteStyle"));
    mActionPasteStyle->setIcon(icon12);
    mActionAddWcsLayer = new QAction(this);
    mActionAddWcsLayer->setObjectName(QString::fromUtf8("mActionAddWcsLayer"));
    QIcon icon108;
    icon108.addFile(QString::fromUtf8(":/images/themes/default/mActionAddWcsLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddWcsLayer->setIcon(icon108);
    mActionDecorationGrid = new QAction(this);
    mActionDecorationGrid->setObjectName(QString::fromUtf8("mActionDecorationGrid"));
    QIcon icon109;
    icon109.addFile(QString::fromUtf8(":/images/themes/default/grid.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecorationGrid->setIcon(icon109);
    mActionPinLabels = new QAction(this);
    mActionPinLabels->setObjectName(QString::fromUtf8("mActionPinLabels"));
    mActionPinLabels->setCheckable(true);
    QIcon icon110;
    icon110.addFile(QString::fromUtf8(":/images/themes/default/mActionPinLabels.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionPinLabels->setIcon(icon110);
    mActionShowPinnedLabels = new QAction(this);
    mActionShowPinnedLabels->setObjectName(QString::fromUtf8("mActionShowPinnedLabels"));
    mActionShowPinnedLabels->setCheckable(true);
    QIcon icon111;
    icon111.addFile(QString::fromUtf8(":/images/themes/default/mActionShowPinnedLabels.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowPinnedLabels->setIcon(icon111);
    mActionNewBlankProject = new QAction(this);
    mActionNewBlankProject->setObjectName(QString::fromUtf8("mActionNewBlankProject"));
    mActionNewBlankProject->setIcon(icon);
    mActionLocalCumulativeCutStretch = new QAction(this);
    mActionLocalCumulativeCutStretch->setObjectName(QString::fromUtf8("mActionLocalCumulativeCutStretch"));
    QIcon icon112;
    icon112.addFile(QString::fromUtf8(":/images/themes/default/mActionLocalCumulativeCutStretch.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionLocalCumulativeCutStretch->setIcon(icon112);
    mActionFullCumulativeCutStretch = new QAction(this);
    mActionFullCumulativeCutStretch->setObjectName(QString::fromUtf8("mActionFullCumulativeCutStretch"));
    QIcon icon113;
    icon113.addFile(QString::fromUtf8(":/images/themes/default/mActionFullCumulativeCutStretch.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionFullCumulativeCutStretch->setIcon(icon113);
    mActionShowHideLabels = new QAction(this);
    mActionShowHideLabels->setObjectName(QString::fromUtf8("mActionShowHideLabels"));
    mActionShowHideLabels->setCheckable(true);
    QIcon icon114;
    icon114.addFile(QString::fromUtf8(":/images/themes/default/mActionShowHideLabels.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowHideLabels->setIcon(icon114);
    mActionHtmlAnnotation = new QAction(this);
    mActionHtmlAnnotation->setObjectName(QString::fromUtf8("mActionHtmlAnnotation"));
    mActionHtmlAnnotation->setCheckable(true);
    QIcon icon115;
    icon115.addFile(QString::fromUtf8(":/images/themes/default/mActionHtmlAnnotation.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionHtmlAnnotation->setIcon(icon115);
    mActionDuplicateLayer = new QAction(this);
    mActionDuplicateLayer->setObjectName(QString::fromUtf8("mActionDuplicateLayer"));
    QIcon icon116;
    icon116.addFile(QString::fromUtf8(":/images/themes/default/mActionDuplicateLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDuplicateLayer->setIcon(icon116);
    mActionSvgAnnotation = new QAction(this);
    mActionSvgAnnotation->setObjectName(QString::fromUtf8("mActionSvgAnnotation"));
    mActionSvgAnnotation->setCheckable(true);
    QIcon icon117;
    icon117.addFile(QString::fromUtf8(":/images/themes/default/mActionSvgAnnotation.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSvgAnnotation->setIcon(icon117);
    mActionSaveAllEdits = new QAction(this);
    mActionSaveAllEdits->setObjectName(QString::fromUtf8("mActionSaveAllEdits"));
    QIcon icon118;
    icon118.addFile(QString::fromUtf8(":/images/themes/default/mActionSaveAllEdits.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSaveAllEdits->setIcon(icon118);
    mActionRollbackAllEdits = new QAction(this);
    mActionRollbackAllEdits->setObjectName(QString::fromUtf8("mActionRollbackAllEdits"));
    QIcon icon119;
    icon119.addFile(QString::fromUtf8(":/images/themes/default/mActionRollbackAllEdits.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionRollbackAllEdits->setIcon(icon119);
    mActionCancelAllEdits = new QAction(this);
    mActionCancelAllEdits->setObjectName(QString::fromUtf8("mActionCancelAllEdits"));
    QIcon icon120;
    icon120.addFile(QString::fromUtf8(":/images/themes/default/mActionCancelAllEdits.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionCancelAllEdits->setIcon(icon120);
    mActionRollbackEdits = new QAction(this);
    mActionRollbackEdits->setObjectName(QString::fromUtf8("mActionRollbackEdits"));
    QIcon icon121;
    icon121.addFile(QString::fromUtf8(":/images/themes/default/mActionRollbackEdits.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionRollbackEdits->setIcon(icon121);
    mActionAllEdits = new QAction(this);
    mActionAllEdits->setObjectName(QString::fromUtf8("mActionAllEdits"));
    QIcon icon122;
    icon122.addFile(QString::fromUtf8(":/images/themes/default/mActionAllEdits.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAllEdits->setIcon(icon122);
    mActionCancelEdits = new QAction(this);
    mActionCancelEdits->setObjectName(QString::fromUtf8("mActionCancelEdits"));
    QIcon icon123;
    icon123.addFile(QString::fromUtf8(":/images/themes/default/mActionCancelEdits.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionCancelEdits->setIcon(icon123);
    mActionSaveLayerEdits = new QAction(this);
    mActionSaveLayerEdits->setObjectName(QString::fromUtf8("mActionSaveLayerEdits"));
    mActionSaveLayerEdits->setIcon(icon118);
    mActionScaleFeature = new QAction(this);
    mActionScaleFeature->setObjectName(QString::fromUtf8("mActionScaleFeature"));
    mActionScaleFeature->setCheckable(true);
    QIcon icon124;
    icon124.addFile(QString::fromUtf8(":/images/themes/default/mActionScaleFeature.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionScaleFeature->setIcon(icon124);
    mActionRotateFeature = new QAction(this);
    mActionRotateFeature->setObjectName(QString::fromUtf8("mActionRotateFeature"));
    mActionRotateFeature->setCheckable(true);
    QIcon icon125;
    icon125.addFile(QString::fromUtf8(":/images/themes/default/mActionRotateFeature.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionRotateFeature->setIcon(icon125);
    mActionIncreaseBrightness = new QAction(this);
    mActionIncreaseBrightness->setObjectName(QString::fromUtf8("mActionIncreaseBrightness"));
    QIcon icon126;
    icon126.addFile(QString::fromUtf8(":/images/themes/default/mActionIncreaseBrightness.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionIncreaseBrightness->setIcon(icon126);
    mActionDecreaseBrightness = new QAction(this);
    mActionDecreaseBrightness->setObjectName(QString::fromUtf8("mActionDecreaseBrightness"));
    QIcon icon127;
    icon127.addFile(QString::fromUtf8(":/images/themes/default/mActionDecreaseBrightness.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecreaseBrightness->setIcon(icon127);
    mActionIncreaseContrast = new QAction(this);
    mActionIncreaseContrast->setObjectName(QString::fromUtf8("mActionIncreaseContrast"));
    QIcon icon128;
    icon128.addFile(QString::fromUtf8(":/images/themes/default/mActionIncreaseContrast.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionIncreaseContrast->setIcon(icon128);
    mActionDecreaseContrast = new QAction(this);
    mActionDecreaseContrast->setObjectName(QString::fromUtf8("mActionDecreaseContrast"));
    QIcon icon129;
    icon129.addFile(QString::fromUtf8(":/images/themes/default/mActionDecreaseContrast.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecreaseContrast->setIcon(icon129);
    mActionSelectByExpression = new QAction(this);
    mActionSelectByExpression->setObjectName(QString::fromUtf8("mActionSelectByExpression"));
    QIcon icon130;
    icon130.addFile(QString::fromUtf8(":/images/themes/default/mIconExpressionSelect.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSelectByExpression->setIcon(icon130);
    mActionNeedSupport = new QAction(this);
    mActionNeedSupport->setObjectName(QString::fromUtf8("mActionNeedSupport"));
    mActionOpenFieldCalc = new QAction(this);
    mActionOpenFieldCalc->setObjectName(QString::fromUtf8("mActionOpenFieldCalc"));
    QIcon icon131;
    icon131.addFile(QString::fromUtf8(":/images/themes/default/mActionCalculateField.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionOpenFieldCalc->setIcon(icon131);
    mActionAddDelimitedText = new QAction(this);
    mActionAddDelimitedText->setObjectName(QString::fromUtf8("mActionAddDelimitedText"));
    QIcon icon132;
    icon132.addFile(QString::fromUtf8(":/images/themes/default/mActionAddDelimitedTextLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddDelimitedText->setIcon(icon132);
    mActionAddVirtualLayer = new QAction(this);
    mActionAddVirtualLayer->setObjectName(QString::fromUtf8("mActionAddVirtualLayer"));
    QIcon icon133;
    icon133.addFile(QString::fromUtf8(":/images/themes/default/mActionAddVirtualLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddVirtualLayer->setIcon(icon133);
    mActionPasteAsNewVector = new QAction(this);
    mActionPasteAsNewVector->setObjectName(QString::fromUtf8("mActionPasteAsNewVector"));
    mActionPasteAsNewMemoryVector = new QAction(this);
    mActionPasteAsNewMemoryVector->setObjectName(QString::fromUtf8("mActionPasteAsNewMemoryVector"));
    mActionDxfExport = new QAction(this);
    mActionDxfExport->setObjectName(QString::fromUtf8("mActionDxfExport"));
    mActionDwgImport = new QAction(this);
    mActionDwgImport->setObjectName(QString::fromUtf8("mActionDwgImport"));
    mActionFillRing = new QAction(this);
    mActionFillRing->setObjectName(QString::fromUtf8("mActionFillRing"));
    mActionFillRing->setCheckable(true);
    QIcon icon134;
    icon134.addFile(QString::fromUtf8(":/images/themes/default/mActionFillRing.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionFillRing->setIcon(icon134);
    mActionAddLayerDefinition = new QAction(this);
    mActionAddLayerDefinition->setObjectName(QString::fromUtf8("mActionAddLayerDefinition"));
    mActionSaveLayerDefinition = new QAction(this);
    mActionSaveLayerDefinition->setObjectName(QString::fromUtf8("mActionSaveLayerDefinition"));
    mActionPreviewModeOff = new QAction(this);
    mActionPreviewModeOff->setObjectName(QString::fromUtf8("mActionPreviewModeOff"));
    mActionPreviewModeOff->setCheckable(true);
    mActionPreviewModeOff->setChecked(true);
    mActionPreviewModeMono = new QAction(this);
    mActionPreviewModeMono->setObjectName(QString::fromUtf8("mActionPreviewModeMono"));
    mActionPreviewModeMono->setCheckable(true);
    mActionPreviewModeGrayscale = new QAction(this);
    mActionPreviewModeGrayscale->setObjectName(QString::fromUtf8("mActionPreviewModeGrayscale"));
    mActionPreviewModeGrayscale->setCheckable(true);
    mActionPreviewProtanope = new QAction(this);
    mActionPreviewProtanope->setObjectName(QString::fromUtf8("mActionPreviewProtanope"));
    mActionPreviewProtanope->setCheckable(true);
    mActionPreviewDeuteranope = new QAction(this);
    mActionPreviewDeuteranope->setObjectName(QString::fromUtf8("mActionPreviewDeuteranope"));
    mActionPreviewDeuteranope->setCheckable(true);
    mActionPreviewTritanope = new QAction(this);
    mActionPreviewTritanope->setObjectName(QString::fromUtf8("mActionPreviewTritanope"));
    mActionPreviewTritanope->setCheckable(true);
    mActionSetLayerScaleVisibility = new QAction(this);
    mActionSetLayerScaleVisibility->setObjectName(QString::fromUtf8("mActionSetLayerScaleVisibility"));
    mActionShowSelectedLayers = new QAction(this);
    mActionShowSelectedLayers->setObjectName(QString::fromUtf8("mActionShowSelectedLayers"));
    QIcon icon135;
    icon135.addFile(QString::fromUtf8(":/images/themes/default/mActionShowSelectedLayers.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowSelectedLayers->setIcon(icon135);
    mActionHideSelectedLayers = new QAction(this);
    mActionHideSelectedLayers->setObjectName(QString::fromUtf8("mActionHideSelectedLayers"));
    QIcon icon136;
    icon136.addFile(QString::fromUtf8(":/images/themes/default/mActionHideSelectedLayers.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionHideSelectedLayers->setIcon(icon136);
    mActionToggleSelectedLayers = new QAction(this);
    mActionToggleSelectedLayers->setObjectName(QString::fromUtf8("mActionToggleSelectedLayers"));
    QIcon icon137;
    icon137.addFile(QString::fromUtf8(":/images/themes/default/mActionToggleSelectedLayers.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionToggleSelectedLayers->setIcon(icon137);
    mActionToggleSelectedLayersIndependently = new QAction(this);
    mActionToggleSelectedLayersIndependently->setObjectName(QString::fromUtf8("mActionToggleSelectedLayersIndependently"));
    mActionHideDeselectedLayers = new QAction(this);
    mActionHideDeselectedLayers->setObjectName(QString::fromUtf8("mActionHideDeselectedLayers"));
    QIcon icon138;
    icon138.addFile(QString::fromUtf8(":/images/themes/default/mActionHideDeselectedLayers.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionHideDeselectedLayers->setIcon(icon138);
    mActionNewMemoryLayer = new QAction(this);
    mActionNewMemoryLayer->setObjectName(QString::fromUtf8("mActionNewMemoryLayer"));
    QIcon icon139;
    icon139.addFile(QString::fromUtf8(":/images/themes/default/mActionCreateMemory.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewMemoryLayer->setIcon(icon139);
    mActionStatisticalSummary = new QAction(this);
    mActionStatisticalSummary->setObjectName(QString::fromUtf8("mActionStatisticalSummary"));
    mActionStatisticalSummary->setCheckable(true);
    QIcon icon140;
    icon140.addFile(QString::fromUtf8(":/images/themes/default/mActionSum.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionStatisticalSummary->setIcon(icon140);
    mActionShowAlignRasterTool = new QAction(this);
    mActionShowAlignRasterTool->setObjectName(QString::fromUtf8("mActionShowAlignRasterTool"));
    mActionReportaBug = new QAction(this);
    mActionReportaBug->setObjectName(QString::fromUtf8("mActionReportaBug"));
    mActionDiagramProperties = new QAction(this);
    mActionDiagramProperties->setObjectName(QString::fromUtf8("mActionDiagramProperties"));
    QIcon icon141;
    icon141.addFile(QString::fromUtf8(":/images/themes/default/propertyicons/diagram.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDiagramProperties->setIcon(icon141);
    mActionNewGeoPackageLayer = new QAction(this);
    mActionNewGeoPackageLayer->setObjectName(QString::fromUtf8("mActionNewGeoPackageLayer"));
    QIcon icon142;
    icon142.addFile(QString::fromUtf8(":/images/themes/default/mActionNewGeoPackageLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewGeoPackageLayer->setIcon(icon142);
    mActionMultiEditAttributes = new QAction(this);
    mActionMultiEditAttributes->setObjectName(QString::fromUtf8("mActionMultiEditAttributes"));
    QIcon icon143;
    icon143.addFile(QString::fromUtf8(":/images/themes/default/mActionMultiEdit.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMultiEditAttributes->setIcon(icon143);
    mActionAddAfsLayer = new QAction(this);
    mActionAddAfsLayer->setObjectName(QString::fromUtf8("mActionAddAfsLayer"));
    QIcon icon144;
    icon144.addFile(QString::fromUtf8(":/images/themes/default/mActionAddAfsLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddAfsLayer->setIcon(icon144);
    mActionSelectByForm = new QAction(this);
    mActionSelectByForm->setObjectName(QString::fromUtf8("mActionSelectByForm"));
    QIcon icon145;
    icon145.addFile(QString::fromUtf8(":/images/themes/default/mIconFormSelect.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionSelectByForm->setIcon(icon145);
    mActionMoveFeatureCopy = new QAction(this);
    mActionMoveFeatureCopy->setObjectName(QString::fromUtf8("mActionMoveFeatureCopy"));
    mActionMoveFeatureCopy->setCheckable(true);
    QIcon icon146;
    icon146.addFile(QString::fromUtf8(":/images/themes/default/mActionMoveFeatureCopy.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMoveFeatureCopy->setIcon(icon146);
    mActionDecorationLayoutExtent = new QAction(this);
    mActionDecorationLayoutExtent->setObjectName(QString::fromUtf8("mActionDecorationLayoutExtent"));
    QIcon icon147;
    icon147.addFile(QString::fromUtf8(":/images/themes/default/mActionAddMap.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecorationLayoutExtent->setIcon(icon147);
    mActionDataSourceManager = new QAction(this);
    mActionDataSourceManager->setObjectName(QString::fromUtf8("mActionDataSourceManager"));
    QIcon icon148;
    icon148.addFile(QString::fromUtf8(":/images/themes/default/mActionDataSourceManager.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDataSourceManager->setIcon(icon148);
    mActionShowLayoutManager = new QAction(this);
    mActionShowLayoutManager->setObjectName(QString::fromUtf8("mActionShowLayoutManager"));
    QIcon icon149;
    icon149.addFile(QString::fromUtf8(":/images/themes/default/mActionLayoutManager.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowLayoutManager->setIcon(icon149);
    mActionNewPrintLayout = new QAction(this);
    mActionNewPrintLayout->setObjectName(QString::fromUtf8("mActionNewPrintLayout"));
    QIcon icon150;
    icon150.addFile(QString::fromUtf8(":/images/themes/default/mActionNewLayout.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewPrintLayout->setIcon(icon150);
    mActionNewReport = new QAction(this);
    mActionNewReport->setObjectName(QString::fromUtf8("mActionNewReport"));
    QIcon icon151;
    icon151.addFile(QString::fromUtf8(":/images/themes/default/mActionNewReport.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewReport->setIcon(icon151);
    mActionCloseProject = new QAction(this);
    mActionCloseProject->setObjectName(QString::fromUtf8("mActionCloseProject"));
    mActionRevertProject = new QAction(this);
    mActionRevertProject->setObjectName(QString::fromUtf8("mActionRevertProject"));
    mActionCopyLayer = new QAction(this);
    mActionCopyLayer->setObjectName(QString::fromUtf8("mActionCopyLayer"));
    mActionCopyLayer->setIcon(icon11);
    mActionPasteLayer = new QAction(this);
    mActionPasteLayer->setObjectName(QString::fromUtf8("mActionPasteLayer"));
    mActionPasteLayer->setIcon(icon12);
    mActionVertexToolActiveLayer = new QAction(this);
    mActionVertexToolActiveLayer->setObjectName(QString::fromUtf8("mActionVertexToolActiveLayer"));
    mActionVertexToolActiveLayer->setCheckable(true);
    mActionVertexToolActiveLayer->setIcon(icon25);
    mActionShowMeshCalculator = new QAction(this);
    mActionShowMeshCalculator->setObjectName(QString::fromUtf8("mActionShowMeshCalculator"));
    QIcon icon152;
    icon152.addFile(QString::fromUtf8(":/images/themes/default/mActionShowMeshCalculator.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowMeshCalculator->setIcon(icon152);
    mActionAddMeshLayer = new QAction(this);
    mActionAddMeshLayer->setObjectName(QString::fromUtf8("mActionAddMeshLayer"));
    QIcon icon153;
    icon153.addFile(QString::fromUtf8(":/images/themes/default/mActionAddMeshLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddMeshLayer->setIcon(icon153);
    mActionNewVirtualLayer = new QAction(this);
    mActionNewVirtualLayer->setObjectName(QString::fromUtf8("mActionNewVirtualLayer"));
    QIcon icon154;
    icon154.addFile(QString::fromUtf8(":/images/themes/default/mActionNewVirtualLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewVirtualLayer->setIcon(icon154);
    mActionShowUnplacedLabels = new QAction(this);
    mActionShowUnplacedLabels->setObjectName(QString::fromUtf8("mActionShowUnplacedLabels"));
    mActionShowUnplacedLabels->setCheckable(true);
    QIcon icon155;
    icon155.addFile(QString::fromUtf8(":/images/themes/default/mActionShowUnplacedLabel.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowUnplacedLabels->setIcon(icon155);
    mActionReselect = new QAction(this);
    mActionReselect->setObjectName(QString::fromUtf8("mActionReselect"));
    mActionTemporalController = new QAction(this);
    mActionTemporalController->setObjectName(QString::fromUtf8("mActionTemporalController"));
    QIcon icon156;
    icon156.addFile(QString::fromUtf8(":/images/themes/default/propertyicons/temporal.svg"), QSize(), QIcon::Normal, QIcon::On);
    mActionTemporalController->setIcon(icon156);
    mActionAddXyzLayer = new QAction(this);
    mActionAddXyzLayer->setObjectName(QString::fromUtf8("mActionAddXyzLayer"));
    QIcon icon157;
    icon157.addFile(QString::fromUtf8(":/images/themes/default/mActionAddXyzLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddXyzLayer->setIcon(icon157);
    mActionAddVectorTileLayer = new QAction(this);
    mActionAddVectorTileLayer->setObjectName(QString::fromUtf8("mActionAddVectorTileLayer"));
    QIcon icon158;
    icon158.addFile(QString::fromUtf8(":/images/themes/default/mActionAddVectorTileLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddVectorTileLayer->setIcon(icon158);
    mActionShowGeoreferencer = new QAction(this);
    mActionShowGeoreferencer->setObjectName(QString::fromUtf8("mActionShowGeoreferencer"));
    QIcon icon159;
    icon159.addFile(QString::fromUtf8(":/images/themes/default/georeferencer/mGeorefRun.png"), QSize(), QIcon::Normal, QIcon::Off);
    mActionShowGeoreferencer->setIcon(icon159);
    mActionDecreaseGamma = new QAction(this);
    mActionDecreaseGamma->setObjectName(QString::fromUtf8("mActionDecreaseGamma"));
    QIcon icon160;
    icon160.addFile(QString::fromUtf8(":/images/themes/default/mActionDecreaseGamma.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDecreaseGamma->setIcon(icon160);
    mActionIncreaseGamma = new QAction(this);
    mActionIncreaseGamma->setObjectName(QString::fromUtf8("mActionIncreaseGamma"));
    QIcon icon161;
    icon161.addFile(QString::fromUtf8(":/images/themes/default/mActionIncreaseGamma.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionIncreaseGamma->setIcon(icon161);
    mActionDigitizeWithCurve = new QAction(this);
    mActionDigitizeWithCurve->setObjectName(QString::fromUtf8("mActionDigitizeWithCurve"));
    mActionDigitizeWithCurve->setCheckable(true);
    mActionDigitizeWithCurve->setEnabled(false);
    QIcon icon162;
    icon162.addFile(QString::fromUtf8(":/images/themes/default/mActionDigitizeWithCurve.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDigitizeWithCurve->setIcon(icon162);
    mActionAddPointCloudLayer = new QAction(this);
    mActionAddPointCloudLayer->setObjectName(QString::fromUtf8("mActionAddPointCloudLayer"));
    QIcon icon163;
    icon163.addFile(QString::fromUtf8(":/images/themes/default/mActionAddPointCloudLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionAddPointCloudLayer->setIcon(icon163);
    mActionStreamDigitize = new QAction(this);
    mActionStreamDigitize->setObjectName(QString::fromUtf8("mActionStreamDigitize"));
    mActionStreamDigitize->setCheckable(true);
    mActionStreamDigitize->setEnabled(true);
    QIcon icon164;
    icon164.addFile(QString::fromUtf8(":/images/themes/default/mActionStreamingDigitize.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionStreamDigitize->setIcon(icon164);
    mActionMeasureBearing = new QAction(this);
    mActionMeasureBearing->setObjectName(QString::fromUtf8("mActionMeasureBearing"));
    mActionMeasureBearing->setCheckable(true);
    QIcon icon165;
    icon165.addFile(QString::fromUtf8(":/images/themes/default/mActionMeasureBearing.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionMeasureBearing->setIcon(icon165);
    mActionNewMeshLayer = new QAction(this);
    mActionNewMeshLayer->setObjectName(QString::fromUtf8("mActionNewMeshLayer"));
    QIcon icon166;
    icon166.addFile(QString::fromUtf8(":/images/themes/default/mActionNewMeshLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewMeshLayer->setIcon(icon166);
    mActionNewGpxLayer = new QAction(this);
    mActionNewGpxLayer->setObjectName(QString::fromUtf8("mActionNewGpxLayer"));
    QIcon icon167;
    icon167.addFile(QString::fromUtf8(":/images/themes/default/mActionNewGpx.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNewGpxLayer->setIcon(icon167);
    mActionCreateAnnotationLayer = new QAction(this);
    mActionCreateAnnotationLayer->setObjectName(QString::fromUtf8("mActionCreateAnnotationLayer"));
    QIcon icon168;
    icon168.addFile(QString::fromUtf8(":/images/themes/default/mActionCreateAnnotationLayer.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionCreateAnnotationLayer->setIcon(icon168);
    mActionModifyAnnotation = new QAction(this);
    mActionModifyAnnotation->setObjectName(QString::fromUtf8("mActionModifyAnnotation"));
    mActionModifyAnnotation->setCheckable(true);
    QIcon icon169;
    icon169.addFile(QString::fromUtf8(":/images/themes/default/mActionSelect.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionModifyAnnotation->setIcon(icon169);
    mMainAnnotationLayerProperties = new QAction(this);
    mMainAnnotationLayerProperties->setObjectName(QString::fromUtf8("mMainAnnotationLayerProperties"));
    mActionNew3DMapCanvas = new QAction(this);
    mActionNew3DMapCanvas->setObjectName(QString::fromUtf8("mActionNew3DMapCanvas"));
    QIcon icon170;
    icon170.addFile(QString::fromUtf8(":/images/themes/default/mActionNew3DMap.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionNew3DMapCanvas->setIcon(icon170);
    mActionManage3DMapViews = new QAction(this);
    mActionManage3DMapViews->setObjectName(QString::fromUtf8("mActionManage3DMapViews"));
    mActionDigitizeWithSegment = new QAction(this);
    mActionDigitizeWithSegment->setObjectName(QString::fromUtf8("mActionDigitizeWithSegment"));
    mActionDigitizeWithSegment->setCheckable(true);
    QIcon icon171;
    icon171.addFile(QString::fromUtf8(":/images/themes/default/mActionDigitizeWithSegment.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDigitizeWithSegment->setIcon(icon171);
    mActionDigitizeShape = new QAction(this);
    mActionDigitizeShape->setObjectName(QString::fromUtf8("mActionDigitizeShape"));
    mActionDigitizeShape->setCheckable(true);
    QIcon icon172;
    icon172.addFile(QString::fromUtf8(":/images/themes/default/mActionDigitizeShape.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionDigitizeShape->setIcon(icon172);
    mActionElevationProfile = new QAction(this);
    mActionElevationProfile->setObjectName(QString::fromUtf8("mActionElevationProfile"));
    QIcon icon173;
    icon173.addFile(QString::fromUtf8(":/images/themes/default/mLayoutItem3DMap.svg"), QSize(), QIcon::Normal, QIcon::Off);
    mActionElevationProfile->setIcon(icon173);

    mActionNewProject->setText(QCoreApplication::translate("MainWindow", "&New", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewProject->setToolTip(QCoreApplication::translate("MainWindow", "New Project", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionNewProject->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionOpenProject->setText(QCoreApplication::translate("MainWindow", "&Open\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionOpenProject->setToolTip(QCoreApplication::translate("MainWindow", "Open Project", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionOpenProject->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionSaveProject->setText(QCoreApplication::translate("MainWindow", "&Save", nullptr));
#if QT_CONFIG(tooltip)
    mActionSaveProject->setToolTip(QCoreApplication::translate("MainWindow", "Save Project", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionSaveProject->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionSaveProjectAs->setText(QCoreApplication::translate("MainWindow", "Save &As\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionSaveProjectAs->setToolTip(QCoreApplication::translate("MainWindow", "Save Project As", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionSaveProjectAs->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionSaveMapAsImage->setText(QCoreApplication::translate("MainWindow", "Export Map to &Image\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionSaveMapAsImage->setToolTip(QCoreApplication::translate("MainWindow", "Save Map as Image", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionSaveMapAsPdf->setText(QCoreApplication::translate("MainWindow", "Export Map to &PDF\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionSaveMapAsPdf->setToolTip(QCoreApplication::translate("MainWindow", "Save Map as PDF", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionNewMapCanvas->setText(QCoreApplication::translate("MainWindow", "New &Map View", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewMapCanvas->setToolTip(QCoreApplication::translate("MainWindow", "New Map View", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionNewMapCanvas->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionExit->setText(QCoreApplication::translate("MainWindow", "Exit QGIS", nullptr));
#if QT_CONFIG(shortcut)
    mActionExit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionUndo->setText(QCoreApplication::translate("MainWindow", "&Undo", nullptr));
#if QT_CONFIG(shortcut)
    mActionUndo->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Z", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionRedo->setText(QCoreApplication::translate("MainWindow", "&Redo", nullptr));
#if QT_CONFIG(shortcut)
    mActionRedo->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+Z", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionCutFeatures->setText(QCoreApplication::translate("MainWindow", "Cut Features", nullptr));
    mActionCopyFeatures->setText(QCoreApplication::translate("MainWindow", "Copy Features", nullptr));
    mActionPasteFeatures->setText(QCoreApplication::translate("MainWindow", "Paste Features", nullptr));
    mActionAddFeature->setText(QCoreApplication::translate("MainWindow", "Add Feature", nullptr));
#if QT_CONFIG(shortcut)
    mActionAddFeature->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+.", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionMoveFeature->setText(QCoreApplication::translate("MainWindow", "Move Feature(s)", nullptr));
    mActionReshapeFeatures->setText(QCoreApplication::translate("MainWindow", "Reshape Features", nullptr));
    mActionSplitFeatures->setText(QCoreApplication::translate("MainWindow", "Split Features", nullptr));
    mActionSplitParts->setText(QCoreApplication::translate("MainWindow", "Split Parts", nullptr));
    mActionDeleteSelected->setText(QCoreApplication::translate("MainWindow", "Delete Selected", nullptr));
    mActionAddRing->setText(QCoreApplication::translate("MainWindow", "Add Ring", nullptr));
    mActionAddPart->setText(QCoreApplication::translate("MainWindow", "Add Part", nullptr));
    mActionSimplifyFeature->setText(QCoreApplication::translate("MainWindow", "Simplify Feature", nullptr));
    mActionDeleteRing->setText(QCoreApplication::translate("MainWindow", "Delete Ring", nullptr));
    mActionDeletePart->setText(QCoreApplication::translate("MainWindow", "Delete Part", nullptr));
    mActionMergeFeatures->setText(QCoreApplication::translate("MainWindow", "Merge Selected Features", nullptr));
    mActionMergeFeatureAttributes->setText(QCoreApplication::translate("MainWindow", "Merge Attributes of Selected Features", nullptr));
    mActionVertexTool->setText(QCoreApplication::translate("MainWindow", "&Vertex Tool (All Layers)", nullptr));
#if QT_CONFIG(tooltip)
    mActionVertexTool->setToolTip(QCoreApplication::translate("MainWindow", "Vertex Tool \342\200\224 All Layers\n"
"Manipulate vertices on any editable layer:\n"
"Right click to lock on a feature.\n"
"Click and drag to select vertices by rectangle.\n"
"Alt+click to select vertices by polygon.\n"
"Shift+click/drag to add vertices to selection.\n"
"Ctrl+click/drag to remove vertices from selection.\n"
"Shift+R to enable range selection.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionRotatePointSymbols->setText(QCoreApplication::translate("MainWindow", "Rotate Point Symbols", nullptr));
    mActionOffsetPointSymbol->setText(QCoreApplication::translate("MainWindow", "Offset Point Symbol", nullptr));
    mActionReverseLine->setText(QCoreApplication::translate("MainWindow", "Reverse line", nullptr));
    mActionTrimExtendFeature->setText(QCoreApplication::translate("MainWindow", "Trim/Extend Feature", nullptr));
    mActionSnappingOptions->setText(QCoreApplication::translate("MainWindow", "&Snapping Options\342\200\246", nullptr));
    mActionPan->setText(QCoreApplication::translate("MainWindow", "Pan Map", nullptr));
    mActionZoomIn->setText(QCoreApplication::translate("MainWindow", "Zoom In", nullptr));
#if QT_CONFIG(shortcut)
    mActionZoomIn->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Alt++", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionZoomOut->setText(QCoreApplication::translate("MainWindow", "Zoom Out", nullptr));
#if QT_CONFIG(shortcut)
    mActionZoomOut->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Alt+-", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionSelectFeatures->setText(QCoreApplication::translate("MainWindow", "Select Feature(s)", nullptr));
#if QT_CONFIG(tooltip)
    mActionSelectFeatures->setToolTip(QCoreApplication::translate("MainWindow", "Select Features by Area or Single Click", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionSelectPolygon->setText(QCoreApplication::translate("MainWindow", "Select Features by Polygon", nullptr));
    mActionSelectFreehand->setText(QCoreApplication::translate("MainWindow", "Select Features by Freehand", nullptr));
    mActionSelectRadius->setText(QCoreApplication::translate("MainWindow", "Select Features by Radius", nullptr));
    mActionDeselectAll->setText(QCoreApplication::translate("MainWindow", "Deselect Features from All Layers", nullptr));
#if QT_CONFIG(shortcut)
    mActionDeselectAll->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Alt+A", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionDeselectActiveLayer->setText(QCoreApplication::translate("MainWindow", "Deselect Features from the Current Active Layer", nullptr));
#if QT_CONFIG(shortcut)
    mActionDeselectActiveLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+A", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionSelectAll->setText(QCoreApplication::translate("MainWindow", "Select All Features", nullptr));
#if QT_CONFIG(tooltip)
    mActionSelectAll->setToolTip(QCoreApplication::translate("MainWindow", "Select All Features", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionInvertSelection->setText(QCoreApplication::translate("MainWindow", "Invert Feature Selection", nullptr));
#if QT_CONFIG(tooltip)
    mActionInvertSelection->setToolTip(QCoreApplication::translate("MainWindow", "Invert Feature Selection", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionIdentify->setText(QCoreApplication::translate("MainWindow", "Identify Features", nullptr));
#if QT_CONFIG(shortcut)
    mActionIdentify->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+I", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionMeasure->setText(QCoreApplication::translate("MainWindow", "Measure Line", nullptr));
#if QT_CONFIG(shortcut)
    mActionMeasure->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+M", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionMeasureArea->setText(QCoreApplication::translate("MainWindow", "Measure Area", nullptr));
#if QT_CONFIG(shortcut)
    mActionMeasureArea->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+J", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionMeasureAngle->setText(QCoreApplication::translate("MainWindow", "Measure Angle", nullptr));
    mActionZoomFullExtent->setText(QCoreApplication::translate("MainWindow", "Zoom &Full", nullptr));
#if QT_CONFIG(shortcut)
    mActionZoomFullExtent->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+F", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionZoomToLayer->setText(QCoreApplication::translate("MainWindow", "Zoom to &Layer", nullptr));
    mActionZoomToLayers->setText(QCoreApplication::translate("MainWindow", "Zoom to &Layer(s)", nullptr));
    mActionZoomToSelected->setText(QCoreApplication::translate("MainWindow", "Zoom to &Selection", nullptr));
#if QT_CONFIG(shortcut)
    mActionZoomToSelected->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+J", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionZoomLast->setText(QCoreApplication::translate("MainWindow", "Zoom Last", nullptr));
    mActionZoomNext->setText(QCoreApplication::translate("MainWindow", "Zoom Next", nullptr));
    mActionZoomActualSize->setText(QCoreApplication::translate("MainWindow", "Zoom to Native Resolution (100%)", nullptr));
#if QT_CONFIG(tooltip)
    mActionZoomActualSize->setToolTip(QCoreApplication::translate("MainWindow", "Zoom to Native Resolution (100%)", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionMapTips->setText(QCoreApplication::translate("MainWindow", "Show Map Tips", nullptr));
#if QT_CONFIG(statustip)
    mActionMapTips->setStatusTip(QCoreApplication::translate("MainWindow", "Show information about a feature when the mouse is hovered over it", nullptr));
#endif // QT_CONFIG(statustip)
    mActionNewBookmark->setText(QCoreApplication::translate("MainWindow", "New Spatial Bookmark\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionNewBookmark->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+B", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionShowBookmarks->setText(QCoreApplication::translate("MainWindow", "Show Spatial Bookmarks", nullptr));
#if QT_CONFIG(shortcut)
    mActionShowBookmarks->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+B", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionShowBookmarkManager->setText(QCoreApplication::translate("MainWindow", "Show Spatial Bookmark Manager", nullptr));
    mActionDraw->setText(QCoreApplication::translate("MainWindow", "Refresh", nullptr));
#if QT_CONFIG(shortcut)
    mActionDraw->setShortcut(QCoreApplication::translate("MainWindow", "F5", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionTextAnnotation->setText(QCoreApplication::translate("MainWindow", "Text Annotation", nullptr));
    mActionFormAnnotation->setText(QCoreApplication::translate("MainWindow", "Form Annotation", nullptr));
    mActionAnnotation->setText(QCoreApplication::translate("MainWindow", "Move Annotation", nullptr));
    mActionLabeling->setText(QCoreApplication::translate("MainWindow", "Labeling", nullptr));
#if QT_CONFIG(tooltip)
    mActionLabeling->setToolTip(QCoreApplication::translate("MainWindow", "Layer Labeling Options", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionNewVectorLayer->setText(QCoreApplication::translate("MainWindow", "New Shapefile Layer\342\200\246", nullptr));
    mActionNewSpatiaLiteLayer->setText(QCoreApplication::translate("MainWindow", "New SpatiaLite Layer\342\200\246", nullptr));
    mActionShowRasterCalculator->setText(QCoreApplication::translate("MainWindow", "Raster Calculator\342\200\246", nullptr));
    mActionAddOgrLayer->setText(QCoreApplication::translate("MainWindow", "Add Vector Layer\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionAddOgrLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+V", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionAddRasterLayer->setText(QCoreApplication::translate("MainWindow", "Add Raster Layer\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionAddRasterLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+R", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionAddPgLayer->setText(QCoreApplication::translate("MainWindow", "Add PostGIS Layers\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionAddPgLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+D", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionAddSpatiaLiteLayer->setText(QCoreApplication::translate("MainWindow", "Add SpatiaLite Layer\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionAddSpatiaLiteLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+L", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionAddMssqlLayer->setText(QCoreApplication::translate("MainWindow", "Add MSSQL Spatial Layer\342\200\246", nullptr));
    mActionAddOracleLayer->setText(QCoreApplication::translate("MainWindow", "Add Oracle Spatial Layer\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionAddOracleLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+O", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionAddHanaLayer->setText(QCoreApplication::translate("MainWindow", "Add SAP HANA Spatial Layer\342\200\246", nullptr));
    mActionAddWmsLayer->setText(QCoreApplication::translate("MainWindow", "Add WMS/WMTS Layer\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionAddWmsLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+W", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionOpenTable->setText(QCoreApplication::translate("MainWindow", "Open &Attribute Table", nullptr));
#if QT_CONFIG(shortcut)
    mActionOpenTable->setShortcut(QCoreApplication::translate("MainWindow", "F6", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionOpenTableSelected->setText(QCoreApplication::translate("MainWindow", "Open Attribute Table (Selected Features)", nullptr));
#if QT_CONFIG(shortcut)
    mActionOpenTableSelected->setShortcut(QCoreApplication::translate("MainWindow", "Shift+F6", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionOpenTableVisible->setText(QCoreApplication::translate("MainWindow", "Open Attribute Table (Visible Features)", nullptr));
#if QT_CONFIG(shortcut)
    mActionOpenTableVisible->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F6", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionOpenTableEdited->setText(QCoreApplication::translate("MainWindow", "Open Attribute Table (Edited and New Features)", nullptr));
    mActionToggleEditing->setText(QCoreApplication::translate("MainWindow", "Toggle &Editing", nullptr));
#if QT_CONFIG(statustip)
    mActionToggleEditing->setStatusTip(QCoreApplication::translate("MainWindow", "Toggles the editing state of the current layer", nullptr));
#endif // QT_CONFIG(statustip)
    mActionSaveEdits->setText(QCoreApplication::translate("MainWindow", "Save for Selected Layer(s)", nullptr));
#if QT_CONFIG(statustip)
    mActionSaveEdits->setStatusTip(QCoreApplication::translate("MainWindow", "Save edits to current layer, but continue editing", nullptr));
#endif // QT_CONFIG(statustip)
    mActionLayerSaveAs->setText(QCoreApplication::translate("MainWindow", "&Save As\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionLayerSaveAs->setToolTip(QCoreApplication::translate("MainWindow", "Save Layer As", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionRemoveLayer->setText(QCoreApplication::translate("MainWindow", "Remove Layer/Group", nullptr));
#if QT_CONFIG(tooltip)
    mActionRemoveLayer->setToolTip(QCoreApplication::translate("MainWindow", "Remove Layer/Group", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionRemoveLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+D", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionSetLayerCRS->setText(QCoreApplication::translate("MainWindow", "Set CRS of Layer(s)", nullptr));
#if QT_CONFIG(shortcut)
    mActionSetLayerCRS->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+C", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionSetProjectCRSFromLayer->setText(QCoreApplication::translate("MainWindow", "Set Project CRS from Layer", nullptr));
    mActionLayerProperties->setText(QCoreApplication::translate("MainWindow", "Layer &Properties\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionLayerProperties->setToolTip(QCoreApplication::translate("MainWindow", "Layer Properties", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionLayerSubsetString->setText(QCoreApplication::translate("MainWindow", "Filter\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionLayerSubsetString->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionAddToOverview->setText(QCoreApplication::translate("MainWindow", "Show in Overview", nullptr));
    mActionAddAllToOverview->setText(QCoreApplication::translate("MainWindow", "Show All in Overview", nullptr));
    mActionRemoveAllFromOverview->setText(QCoreApplication::translate("MainWindow", "Hide All from Overview", nullptr));
    mActionShowAllLayers->setText(QCoreApplication::translate("MainWindow", "Show All Layers", nullptr));
#if QT_CONFIG(shortcut)
    mActionShowAllLayers->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+U", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionHideAllLayers->setText(QCoreApplication::translate("MainWindow", "Hide All Layers", nullptr));
#if QT_CONFIG(shortcut)
    mActionHideAllLayers->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+H", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionManagePlugins->setText(QCoreApplication::translate("MainWindow", "Manage and Install Plugins\342\200\246", nullptr));
    mActionToggleFullScreen->setText(QCoreApplication::translate("MainWindow", "Toggle Full Scr&een Mode", nullptr));
#if QT_CONFIG(shortcut)
    mActionToggleFullScreen->setShortcut(QCoreApplication::translate("MainWindow", "F11", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionTogglePanelsVisibility->setText(QCoreApplication::translate("MainWindow", "Toggle Panel &Visibility", nullptr));
#if QT_CONFIG(shortcut)
    mActionTogglePanelsVisibility->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Tab", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionToggleMapOnly->setText(QCoreApplication::translate("MainWindow", "Toggle Map Only", nullptr));
#if QT_CONFIG(shortcut)
    mActionToggleMapOnly->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+Tab", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionProjectProperties->setText(QCoreApplication::translate("MainWindow", "&Properties\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionProjectProperties->setToolTip(QCoreApplication::translate("MainWindow", "Project Properties", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionProjectProperties->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+P", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionOptions->setText(QCoreApplication::translate("MainWindow", "&Options\342\200\246", nullptr));
    mActionCustomProjection->setText(QCoreApplication::translate("MainWindow", "Custom Projections\342\200\246", nullptr));
    mActionConfigureShortcuts->setText(QCoreApplication::translate("MainWindow", "Keyboard Shortcuts\342\200\246", nullptr));
    mActionLocalHistogramStretch->setText(QCoreApplication::translate("MainWindow", "Local Histogram Stretch", nullptr));
#if QT_CONFIG(statustip)
    mActionLocalHistogramStretch->setStatusTip(QCoreApplication::translate("MainWindow", "Stretch histogram of active raster to view extents", nullptr));
#endif // QT_CONFIG(statustip)
    mActionHelpContents->setText(QCoreApplication::translate("MainWindow", "Help Contents", nullptr));
#if QT_CONFIG(shortcut)
    mActionHelpContents->setShortcut(QCoreApplication::translate("MainWindow", "F1", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionHelpAPI->setText(QCoreApplication::translate("MainWindow", "API Documentation", nullptr));
    mActionQgisHomePage->setText(QCoreApplication::translate("MainWindow", "QGIS Home Page", nullptr));
#if QT_CONFIG(shortcut)
    mActionQgisHomePage->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+H", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionCheckQgisVersion->setText(QCoreApplication::translate("MainWindow", "Check QGIS Version", nullptr));
#if QT_CONFIG(statustip)
    mActionCheckQgisVersion->setStatusTip(QCoreApplication::translate("MainWindow", "Check if your QGIS version is up to date (requires internet access)", nullptr));
#endif // QT_CONFIG(statustip)
    mActionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
    mActionSponsors->setText(QCoreApplication::translate("MainWindow", "QGIS Sustaining Members", nullptr));
    mActionMoveLabel->setText(QCoreApplication::translate("MainWindow", "Move Label, Diagram or Callout", nullptr));
#if QT_CONFIG(tooltip)
    mActionMoveLabel->setToolTip(QCoreApplication::translate("MainWindow", "Move a Label, Diagram or Callout\n"
"Acts on all layers.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionRotateLabel->setText(QCoreApplication::translate("MainWindow", "Rotate Label", nullptr));
#if QT_CONFIG(tooltip)
    mActionRotateLabel->setToolTip(QCoreApplication::translate("MainWindow", "Rotate a Label\n"
"Holding Ctrl (Cmd) while rotating increments by 15 deg.\n"
"Acts on all layers.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionChangeLabelProperties->setText(QCoreApplication::translate("MainWindow", "Change Label", nullptr));
#if QT_CONFIG(tooltip)
    mActionChangeLabelProperties->setToolTip(QCoreApplication::translate("MainWindow", "Change Label Properties\n"
"Allows customization of individual label properties such as font size and color.\n"
"Acts on all layers.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionStyleManager->setText(QCoreApplication::translate("MainWindow", "Style Manager\342\200\246", nullptr));
    mActionShowPythonDialog->setText(QCoreApplication::translate("MainWindow", "Python Console", nullptr));
#if QT_CONFIG(shortcut)
    mActionShowPythonDialog->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Alt+P", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionFullHistogramStretch->setText(QCoreApplication::translate("MainWindow", "Full Histogram Stretch", nullptr));
#if QT_CONFIG(tooltip)
    mActionFullHistogramStretch->setToolTip(QCoreApplication::translate("MainWindow", "Stretch Histogram to Full Dataset", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionCustomization->setText(QCoreApplication::translate("MainWindow", "Interface Customization\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    actionActionCatchForCustomization->setToolTip(QCoreApplication::translate("MainWindow", "This is here just to avoid shortcut conflicts, the shortcut is caught in QgsCustomization", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    actionActionCatchForCustomization->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionEmbedLayers->setText(QCoreApplication::translate("MainWindow", "Embed Layers and Groups\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionEmbedLayers->setToolTip(QCoreApplication::translate("MainWindow", "Embed Layers and Groups from other Project Files", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionDecorationTitle->setText(QCoreApplication::translate("MainWindow", "&Title Label\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionDecorationTitle->setToolTip(QCoreApplication::translate("MainWindow", "Title Label", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
    mActionDecorationTitle->setWhatsThis(QCoreApplication::translate("MainWindow", "Creates a title label that is displayed on the map canvas.", nullptr));
#endif // QT_CONFIG(whatsthis)
    mActionDecorationCopyright->setText(QCoreApplication::translate("MainWindow", "&Copyright Label\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionDecorationCopyright->setToolTip(QCoreApplication::translate("MainWindow", "Copyright Label", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
    mActionDecorationCopyright->setWhatsThis(QCoreApplication::translate("MainWindow", "Creates a copyright label that is displayed on the map canvas.", nullptr));
#endif // QT_CONFIG(whatsthis)
    mActionDecorationImage->setText(QCoreApplication::translate("MainWindow", "&Image\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionDecorationImage->setToolTip(QCoreApplication::translate("MainWindow", "Image", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
    mActionDecorationImage->setWhatsThis(QCoreApplication::translate("MainWindow", "\"Creates an image that is displayed on the map canvas\"", nullptr));
#endif // QT_CONFIG(whatsthis)
    mActionDecorationNorthArrow->setText(QCoreApplication::translate("MainWindow", "&North Arrow\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionDecorationNorthArrow->setToolTip(QCoreApplication::translate("MainWindow", "North Arrow", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
    mActionDecorationNorthArrow->setWhatsThis(QCoreApplication::translate("MainWindow", "\"Creates a north arrow that is displayed on the map canvas\"", nullptr));
#endif // QT_CONFIG(whatsthis)
    mActionDecorationScaleBar->setText(QCoreApplication::translate("MainWindow", "&Scale Bar\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionDecorationScaleBar->setToolTip(QCoreApplication::translate("MainWindow", "Scale Bar", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
    mActionDecorationScaleBar->setWhatsThis(QCoreApplication::translate("MainWindow", "Creates a scale bar that is displayed on the map canvas", nullptr));
#endif // QT_CONFIG(whatsthis)
    mActionAddWfsLayer->setText(QCoreApplication::translate("MainWindow", "Add WFS Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionAddWfsLayer->setToolTip(QCoreApplication::translate("MainWindow", "Add WFS Layer", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionFeatureAction->setText(QCoreApplication::translate("MainWindow", "Feature Action", nullptr));
#if QT_CONFIG(tooltip)
    mActionFeatureAction->setToolTip(QCoreApplication::translate("MainWindow", "Run Feature Action", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPanToSelected->setText(QCoreApplication::translate("MainWindow", "Pan Map to Selection", nullptr));
#if QT_CONFIG(tooltip)
    mActionPanToSelected->setToolTip(QCoreApplication::translate("MainWindow", "Pan Map to Selection", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionOffsetCurve->setText(QCoreApplication::translate("MainWindow", "Offset Curve", nullptr));
    mActionCopyStyle->setText(QCoreApplication::translate("MainWindow", "Copy Style", nullptr));
#if QT_CONFIG(tooltip)
    mActionCopyStyle->setToolTip(QCoreApplication::translate("MainWindow", "Copy Style", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPasteStyle->setText(QCoreApplication::translate("MainWindow", "Paste Style", nullptr));
#if QT_CONFIG(tooltip)
    mActionPasteStyle->setToolTip(QCoreApplication::translate("MainWindow", "Paste Style", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionAddWcsLayer->setText(QCoreApplication::translate("MainWindow", "Add WCS Layer\342\200\246", nullptr));
    mActionDecorationGrid->setText(QCoreApplication::translate("MainWindow", "&Grid\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionDecorationGrid->setToolTip(QCoreApplication::translate("MainWindow", "Grid", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(whatsthis)
    mActionDecorationGrid->setWhatsThis(QCoreApplication::translate("MainWindow", "Creates a grid that is displayed on the map canvas", nullptr));
#endif // QT_CONFIG(whatsthis)
    mActionPinLabels->setText(QCoreApplication::translate("MainWindow", "Pin/Unpin Labels and Diagrams", nullptr));
#if QT_CONFIG(tooltip)
    mActionPinLabels->setToolTip(QCoreApplication::translate("MainWindow", "Pin/Unpin Labels and Diagrams\n"
"Click or marquee on label/diagram to pin.\n"
"Shift clicking or marqueeing unpins labels and diagrams, while Ctrl (Cmd) toggles their state.\n"
"Acts on all layers.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionShowPinnedLabels->setText(QCoreApplication::translate("MainWindow", "Highlight Pinned Labels, Diagrams and Callouts", nullptr));
#if QT_CONFIG(tooltip)
    mActionShowPinnedLabels->setToolTip(QCoreApplication::translate("MainWindow", "Highlight Pinned Labels, Diagrams and Callouts\n"
"Shows highlight rectangles around labels, diagrams and callout points which are fixed in place, e.g. due to manual placement.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionNewBlankProject->setText(QCoreApplication::translate("MainWindow", "New Blank Project", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewBlankProject->setToolTip(QCoreApplication::translate("MainWindow", "New Blank Project", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionLocalCumulativeCutStretch->setText(QCoreApplication::translate("MainWindow", "Local Cumulative Cut Stretch", nullptr));
#if QT_CONFIG(tooltip)
    mActionLocalCumulativeCutStretch->setToolTip(QCoreApplication::translate("MainWindow", "Local cumulative cut stretch using current extent, default limits and estimated values.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionFullCumulativeCutStretch->setText(QCoreApplication::translate("MainWindow", "Full Dataset Cumulative Cut Stretch", nullptr));
#if QT_CONFIG(tooltip)
    mActionFullCumulativeCutStretch->setToolTip(QCoreApplication::translate("MainWindow", "Cumulative cut stretch using full dataset extent, default limits and estimated values.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionShowHideLabels->setText(QCoreApplication::translate("MainWindow", "Show/Hide Labels and Diagrams", nullptr));
#if QT_CONFIG(tooltip)
    mActionShowHideLabels->setToolTip(QCoreApplication::translate("MainWindow", "Show/Hide Labels and Diagrams\n"
"Click or marquee on features to show individual labels or diagrams.\n"
"Shift+click or marquee on a label or diagram to hide it.\n"
"Acts on the currently active layer only.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionHtmlAnnotation->setText(QCoreApplication::translate("MainWindow", "HTML Annotation", nullptr));
#if QT_CONFIG(tooltip)
    mActionHtmlAnnotation->setToolTip(QCoreApplication::translate("MainWindow", "HTML Annotation", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionDuplicateLayer->setText(QCoreApplication::translate("MainWindow", "Duplicate Layer(s)", nullptr));
#if QT_CONFIG(tooltip)
    mActionDuplicateLayer->setToolTip(QCoreApplication::translate("MainWindow", "Duplicate Layer(s)", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionSvgAnnotation->setText(QCoreApplication::translate("MainWindow", "SVG Annotation", nullptr));
    mActionSaveAllEdits->setText(QCoreApplication::translate("MainWindow", "Save for All Layers", nullptr));
#if QT_CONFIG(tooltip)
    mActionSaveAllEdits->setToolTip(QCoreApplication::translate("MainWindow", "Save for All Layers", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
    mActionSaveAllEdits->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
    mActionRollbackAllEdits->setText(QCoreApplication::translate("MainWindow", "Rollback for All Layers", nullptr));
#if QT_CONFIG(tooltip)
    mActionRollbackAllEdits->setToolTip(QCoreApplication::translate("MainWindow", "Rollback for All Layers", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
    mActionRollbackAllEdits->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
    mActionCancelAllEdits->setText(QCoreApplication::translate("MainWindow", "Cancel for All Layers", nullptr));
#if QT_CONFIG(tooltip)
    mActionCancelAllEdits->setToolTip(QCoreApplication::translate("MainWindow", "Cancel for All Layers", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
    mActionCancelAllEdits->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
    mActionRollbackEdits->setText(QCoreApplication::translate("MainWindow", "Rollback for Selected Layer(s)", nullptr));
#if QT_CONFIG(tooltip)
    mActionRollbackEdits->setToolTip(QCoreApplication::translate("MainWindow", "Rollback for Selected Layer(s)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
    mActionRollbackEdits->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
    mActionAllEdits->setText(QCoreApplication::translate("MainWindow", "Current Edits", nullptr));
#if QT_CONFIG(tooltip)
    mActionAllEdits->setToolTip(QCoreApplication::translate("MainWindow", "Current Edits", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
    mActionAllEdits->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
    mActionCancelEdits->setText(QCoreApplication::translate("MainWindow", "Cancel for Selected Layer(s)", nullptr));
#if QT_CONFIG(tooltip)
    mActionCancelEdits->setToolTip(QCoreApplication::translate("MainWindow", "Cancel for Selected Layer(s)", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
    mActionCancelEdits->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
    mActionSaveLayerEdits->setText(QCoreApplication::translate("MainWindow", "Save Layer Edits", nullptr));
#if QT_CONFIG(tooltip)
    mActionSaveLayerEdits->setToolTip(QCoreApplication::translate("MainWindow", "Save Layer Edits", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionScaleFeature->setText(QCoreApplication::translate("MainWindow", "Scale Feature(s)", nullptr));
#if QT_CONFIG(tooltip)
    mActionScaleFeature->setToolTip(QCoreApplication::translate("MainWindow", "Scale Feature(s)\n"
"Ctrl+click to set the base point.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionRotateFeature->setText(QCoreApplication::translate("MainWindow", "Rotate Feature(s)", nullptr));
#if QT_CONFIG(tooltip)
    mActionRotateFeature->setToolTip(QCoreApplication::translate("MainWindow", "Rotate Feature(s)\n"
"Ctrl+click to set the center of rotation.\n"
"Shift+click to snap rotation to 45 degree steps.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionIncreaseBrightness->setText(QCoreApplication::translate("MainWindow", "Increase Brightness", nullptr));
    mActionDecreaseBrightness->setText(QCoreApplication::translate("MainWindow", "Decrease Brightness", nullptr));
    mActionIncreaseContrast->setText(QCoreApplication::translate("MainWindow", "Increase Contrast", nullptr));
    mActionDecreaseContrast->setText(QCoreApplication::translate("MainWindow", "Decrease Contrast", nullptr));
    mActionSelectByExpression->setText(QCoreApplication::translate("MainWindow", "Select Features by Expression\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionSelectByExpression->setToolTip(QCoreApplication::translate("MainWindow", "Select Features Using an Expression", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionSelectByExpression->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F3", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionNeedSupport->setText(QCoreApplication::translate("MainWindow", "Need Commercial Support?", nullptr));
    mActionOpenFieldCalc->setText(QCoreApplication::translate("MainWindow", "Open Field Calculator\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionOpenFieldCalc->setToolTip(QCoreApplication::translate("MainWindow", "Open Field Calculator", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionAddDelimitedText->setText(QCoreApplication::translate("MainWindow", "Add Delimited Text Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionAddDelimitedText->setToolTip(QCoreApplication::translate("MainWindow", "Add Delimited Text Layer", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionAddDelimitedText->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+T", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionAddVirtualLayer->setText(QCoreApplication::translate("MainWindow", "Add/Edit Virtual Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionAddVirtualLayer->setToolTip(QCoreApplication::translate("MainWindow", "Add/Edit Virtual Layer", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPasteAsNewVector->setText(QCoreApplication::translate("MainWindow", "New Vector Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionPasteAsNewVector->setToolTip(QCoreApplication::translate("MainWindow", "Paste features in clipboard into a new OGR vector layer.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPasteAsNewMemoryVector->setText(QCoreApplication::translate("MainWindow", "Temporary Scratch Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionPasteAsNewMemoryVector->setToolTip(QCoreApplication::translate("MainWindow", "Paste features in clipboard into a new temporary scratch layer.", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionPasteAsNewMemoryVector->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Alt+V", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionDxfExport->setText(QCoreApplication::translate("MainWindow", "Export Project to DXF\342\200\246", nullptr));
    mActionDwgImport->setText(QCoreApplication::translate("MainWindow", "Import Layers from DWG/DXF\342\200\246", nullptr));
    mActionFillRing->setText(QCoreApplication::translate("MainWindow", "Fill Ring", nullptr));
#if QT_CONFIG(tooltip)
    mActionFillRing->setToolTip(QCoreApplication::translate("MainWindow", "Fill Ring", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionAddLayerDefinition->setText(QCoreApplication::translate("MainWindow", "Add from Layer Definition File\342\200\246", nullptr));
    mActionSaveLayerDefinition->setText(QCoreApplication::translate("MainWindow", "Save As Layer Definition File\342\200\246", nullptr));
    mActionPreviewModeOff->setText(QCoreApplication::translate("MainWindow", "Normal", nullptr));
#if QT_CONFIG(tooltip)
    mActionPreviewModeOff->setToolTip(QCoreApplication::translate("MainWindow", "Normal Preview Mode", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPreviewModeMono->setText(QCoreApplication::translate("MainWindow", "Simulate Monochrome", nullptr));
#if QT_CONFIG(tooltip)
    mActionPreviewModeMono->setToolTip(QCoreApplication::translate("MainWindow", "Simulate Monochrome", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPreviewModeGrayscale->setText(QCoreApplication::translate("MainWindow", "Simulate Achromatopsia Color Blindness (Grayscale)", nullptr));
#if QT_CONFIG(tooltip)
    mActionPreviewModeGrayscale->setToolTip(QCoreApplication::translate("MainWindow", "Simulate Achromatopsia Color Blindness (Grayscale)", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPreviewProtanope->setText(QCoreApplication::translate("MainWindow", "Simulate Protanopia Color Blindness (No Red)", nullptr));
#if QT_CONFIG(tooltip)
    mActionPreviewProtanope->setToolTip(QCoreApplication::translate("MainWindow", "Simulate Protanopia Color Blindness (No Red)", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPreviewDeuteranope->setText(QCoreApplication::translate("MainWindow", "Simulate Deuteranopia Color Blindness (No Green)", nullptr));
#if QT_CONFIG(tooltip)
    mActionPreviewDeuteranope->setToolTip(QCoreApplication::translate("MainWindow", "Simulate Deuteranopia Color Blindness (No Green)", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionPreviewTritanope->setText(QCoreApplication::translate("MainWindow", "Simulate Tritanopia Color Blindness (No Blue)", nullptr));
#if QT_CONFIG(tooltip)
    mActionPreviewTritanope->setToolTip(QCoreApplication::translate("MainWindow", "Simulate Tritanopia Color Blindness (No Blue)", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionSetLayerScaleVisibility->setText(QCoreApplication::translate("MainWindow", "Set Scale Visibility of Layer(s)", nullptr));
    mActionShowSelectedLayers->setText(QCoreApplication::translate("MainWindow", "Show Selected Layers", nullptr));
    mActionHideSelectedLayers->setText(QCoreApplication::translate("MainWindow", "Hide Selected Layers", nullptr));
    mActionToggleSelectedLayers->setText(QCoreApplication::translate("MainWindow", "Toggle Selected Layers", nullptr));
    mActionToggleSelectedLayersIndependently->setText(QCoreApplication::translate("MainWindow", "Toggle Selected Layers Independently", nullptr));
    mActionHideDeselectedLayers->setText(QCoreApplication::translate("MainWindow", "Hide Deselected Layers", nullptr));
    mActionNewMemoryLayer->setText(QCoreApplication::translate("MainWindow", "New Temporary Scratch Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewMemoryLayer->setToolTip(QCoreApplication::translate("MainWindow", "New Temporary Scratch Layer", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionStatisticalSummary->setText(QCoreApplication::translate("MainWindow", "Statistical Summary", nullptr));
#if QT_CONFIG(tooltip)
    mActionStatisticalSummary->setToolTip(QCoreApplication::translate("MainWindow", "Show Statistical Summary", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionShowAlignRasterTool->setText(QCoreApplication::translate("MainWindow", "Align Rasters\342\200\246", nullptr));
    mActionReportaBug->setText(QCoreApplication::translate("MainWindow", "Report an Issue", nullptr));
    mActionDiagramProperties->setText(QCoreApplication::translate("MainWindow", "Diagram Options", nullptr));
#if QT_CONFIG(tooltip)
    mActionDiagramProperties->setToolTip(QCoreApplication::translate("MainWindow", "Layer Diagram Options", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionNewGeoPackageLayer->setText(QCoreApplication::translate("MainWindow", "New GeoPackage Layer\342\200\246", nullptr));
#if QT_CONFIG(shortcut)
    mActionNewGeoPackageLayer->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+N", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionMultiEditAttributes->setText(QCoreApplication::translate("MainWindow", "Modify Attributes of Selected Features", nullptr));
#if QT_CONFIG(tooltip)
    mActionMultiEditAttributes->setToolTip(QCoreApplication::translate("MainWindow", "Modify the Attributes of all Selected Features Simultaneously", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionAddAfsLayer->setText(QCoreApplication::translate("MainWindow", "Add Ar&cGIS REST Server Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionAddAfsLayer->setToolTip(QCoreApplication::translate("MainWindow", "Add ArcGIS REST Server Layer", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionSelectByForm->setText(QCoreApplication::translate("MainWindow", "Select Features by Value\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionSelectByForm->setToolTip(QCoreApplication::translate("MainWindow", "Select Features by Value", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionSelectByForm->setShortcut(QCoreApplication::translate("MainWindow", "F3", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionMoveFeatureCopy->setText(QCoreApplication::translate("MainWindow", "Copy and Move Feature(s)", nullptr));
#if QT_CONFIG(tooltip)
    mActionMoveFeatureCopy->setToolTip(QCoreApplication::translate("MainWindow", "Copy and Move Feature(s)", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionDecorationLayoutExtent->setText(QCoreApplication::translate("MainWindow", "&Layout Extents\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionDecorationLayoutExtent->setToolTip(QCoreApplication::translate("MainWindow", "Layout Extents", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionDataSourceManager->setText(QCoreApplication::translate("MainWindow", "&Data Source Manager", nullptr));
#if QT_CONFIG(tooltip)
    mActionDataSourceManager->setToolTip(QCoreApplication::translate("MainWindow", "Open Data Source Manager", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionDataSourceManager->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+L", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionShowLayoutManager->setText(QCoreApplication::translate("MainWindow", "Layout Manager\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionShowLayoutManager->setToolTip(QCoreApplication::translate("MainWindow", "Show Layout Manager", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionNewPrintLayout->setText(QCoreApplication::translate("MainWindow", "New &Print Layout\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewPrintLayout->setToolTip(QCoreApplication::translate("MainWindow", "New Print Layout", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionNewPrintLayout->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionNewReport->setText(QCoreApplication::translate("MainWindow", "New &Report\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewReport->setToolTip(QCoreApplication::translate("MainWindow", "New Report", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionCloseProject->setText(QCoreApplication::translate("MainWindow", "Close", nullptr));
#if QT_CONFIG(tooltip)
    mActionCloseProject->setToolTip(QCoreApplication::translate("MainWindow", "Close Project", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionRevertProject->setText(QCoreApplication::translate("MainWindow", "Revert\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionRevertProject->setToolTip(QCoreApplication::translate("MainWindow", "Revert Project to Saved Version", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionCopyLayer->setText(QCoreApplication::translate("MainWindow", "Copy Layer", nullptr));
    mActionPasteLayer->setText(QCoreApplication::translate("MainWindow", "Paste Layer/Group", nullptr));
#if QT_CONFIG(tooltip)
    mActionPasteLayer->setToolTip(QCoreApplication::translate("MainWindow", "Paste Layer/Group", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionVertexToolActiveLayer->setText(QCoreApplication::translate("MainWindow", "&Vertex Tool (Current Layer)", nullptr));
#if QT_CONFIG(tooltip)
    mActionVertexToolActiveLayer->setToolTip(QCoreApplication::translate("MainWindow", "Vertex Tool \342\200\224 Current Layer\n"
"Manipulate vertices on the active layer:\n"
"Right click to lock on a feature.\n"
"Click and drag to select vertices by rectangle.\n"
"Alt+click to select vertices by polygon.\n"
"Shift+click/drag to add vertices to selection.\n"
"Ctrl+click/drag to remove vertices from selection.\n"
"Shift+R to enable range selection.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionShowMeshCalculator->setText(QCoreApplication::translate("MainWindow", "Mesh Calculator\342\200\246", nullptr));
    mActionAddMeshLayer->setText(QCoreApplication::translate("MainWindow", "Add Mesh Layer...", nullptr));
    mActionNewVirtualLayer->setText(QCoreApplication::translate("MainWindow", "New Virtual Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewVirtualLayer->setToolTip(QCoreApplication::translate("MainWindow", "New Virtual Layer", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionShowUnplacedLabels->setText(QCoreApplication::translate("MainWindow", "Show Unplaced Labels", nullptr));
#if QT_CONFIG(tooltip)
    mActionShowUnplacedLabels->setToolTip(QCoreApplication::translate("MainWindow", "Toggle Display of Unplaced Labels\n"
"Shows placeholders for labels which could not be placed, e.g. due to overlaps with other map labels.", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionReselect->setText(QCoreApplication::translate("MainWindow", "Reselect Features", nullptr));
#if QT_CONFIG(tooltip)
    mActionReselect->setToolTip(QCoreApplication::translate("MainWindow", "Reselect Features from Current Layer", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionTemporalController->setText(QCoreApplication::translate("MainWindow", "Temporal Controller", nullptr));
#if QT_CONFIG(tooltip)
    mActionTemporalController->setToolTip(QCoreApplication::translate("MainWindow", "Temporal Controller Panel", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionAddXyzLayer->setText(QCoreApplication::translate("MainWindow", "Add XYZ Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionAddXyzLayer->setToolTip(QCoreApplication::translate("MainWindow", "Add XYZ Layer\342\200\246", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionAddVectorTileLayer->setText(QCoreApplication::translate("MainWindow", "Add Vector Tile Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionAddVectorTileLayer->setToolTip(QCoreApplication::translate("MainWindow", "Add Vector Tile Layer\342\200\246", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionShowGeoreferencer->setText(QCoreApplication::translate("MainWindow", "Georeferencer\342\200\246", nullptr));
    mActionDecreaseGamma->setText(QCoreApplication::translate("MainWindow", "Decrease Gamma", nullptr));
#if QT_CONFIG(tooltip)
    mActionDecreaseGamma->setToolTip(QCoreApplication::translate("MainWindow", "Decrease Gamma", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionIncreaseGamma->setText(QCoreApplication::translate("MainWindow", "Increase Gamma", nullptr));
#if QT_CONFIG(tooltip)
    mActionIncreaseGamma->setToolTip(QCoreApplication::translate("MainWindow", "Increase Gamma", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionDigitizeWithCurve->setText(QCoreApplication::translate("MainWindow", "Digitize with Curve", nullptr));
#if QT_CONFIG(tooltip)
    mActionDigitizeWithCurve->setToolTip(QCoreApplication::translate("MainWindow", "Digitize with curve", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionDigitizeWithCurve->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Shift+G", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionAddPointCloudLayer->setText(QCoreApplication::translate("MainWindow", "Add Point Cloud Layer...", nullptr));
    mActionStreamDigitize->setText(QCoreApplication::translate("MainWindow", "Stream Digitizing", nullptr));
#if QT_CONFIG(tooltip)
    mActionStreamDigitize->setToolTip(QCoreApplication::translate("MainWindow", "Toggles stream digitizing mode", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionMeasureBearing->setText(QCoreApplication::translate("MainWindow", "Measure Bearing", nullptr));
#if QT_CONFIG(tooltip)
    mActionMeasureBearing->setToolTip(QCoreApplication::translate("MainWindow", "Measure Bearing", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionNewMeshLayer->setText(QCoreApplication::translate("MainWindow", "New Mesh Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewMeshLayer->setToolTip(QCoreApplication::translate("MainWindow", "New Mesh Layer", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionNewGpxLayer->setText(QCoreApplication::translate("MainWindow", "New GPX Layer\342\200\246", nullptr));
#if QT_CONFIG(tooltip)
    mActionNewGpxLayer->setToolTip(QCoreApplication::translate("MainWindow", "New GPX Layer", nullptr));
#endif // QT_CONFIG(tooltip)
    mActionCreateAnnotationLayer->setText(QCoreApplication::translate("MainWindow", "New Annotation Layer", nullptr));
    mActionModifyAnnotation->setText(QCoreApplication::translate("MainWindow", "Modify Annotations", nullptr));
#if QT_CONFIG(tooltip)
    mActionModifyAnnotation->setToolTip(QCoreApplication::translate("MainWindow", "Modify Annotations", nullptr));
#endif // QT_CONFIG(tooltip)
    mMainAnnotationLayerProperties->setText(QCoreApplication::translate("MainWindow", "Main Annotation Layer Properties\342\200\246", nullptr));
    mActionNew3DMapCanvas->setText(QCoreApplication::translate("MainWindow", "New &3D Map View", nullptr));
#if QT_CONFIG(tooltip)
    mActionNew3DMapCanvas->setToolTip(QCoreApplication::translate("MainWindow", "New 3D Map View", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
    mActionNew3DMapCanvas->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Alt+M", nullptr));
#endif // QT_CONFIG(shortcut)
    mActionManage3DMapViews->setText(QCoreApplication::translate("MainWindow", "Manage 3D Map Views", nullptr));
    mActionDigitizeWithSegment->setText(QCoreApplication::translate("MainWindow", "Digitize with Segment", nullptr));
    mActionDigitizeShape->setText(QCoreApplication::translate("MainWindow", "Digitize Shape", nullptr));
    mActionElevationProfile->setText(QCoreApplication::translate("MainWindow", "Elevation Profile", nullptr));
#if QT_CONFIG(tooltip)
    mActionElevationProfile->setToolTip(QCoreApplication::translate("MainWindow", "Open an interactive elevation profile chart", nullptr));
#endif // QT_CONFIG(tooltip)
}

void QgisApp::setupMenus(QMainWindow *mainWindow)
{
    menubar = mainWindow->menuBar()->addMenu("GIS Map"); // SG add

    menubar->setObjectName(QString::fromUtf8("menubar"));
    menubar->setGeometry(QRect(0, 0, 1277, 22));
    mProjectMenu = new QMenu(menubar);
    mProjectMenu->setObjectName(QString::fromUtf8("mProjectMenu"));
    mRecentProjectsMenu = new QMenu(mProjectMenu);
    mRecentProjectsMenu->setObjectName(QString::fromUtf8("mRecentProjectsMenu"));
    mLayoutsMenu = new QMenu(mProjectMenu);
    mLayoutsMenu->setObjectName(QString::fromUtf8("mLayoutsMenu"));
    mProjectFromTemplateMenu = new QMenu(mProjectMenu);
    mProjectFromTemplateMenu->setObjectName(QString::fromUtf8("mProjectFromTemplateMenu"));
    mProjectToStorageMenu = new QMenu(mProjectMenu);
    mProjectToStorageMenu->setObjectName(QString::fromUtf8("mProjectToStorageMenu"));
    mProjectFromStorageMenu = new QMenu(mProjectMenu);
    mProjectFromStorageMenu->setObjectName(QString::fromUtf8("mProjectFromStorageMenu"));
    menuImport_Export = new QMenu(mProjectMenu);
    menuImport_Export->setObjectName(QString::fromUtf8("menuImport_Export"));
    mViewMenu = new QMenu(menubar);
    mViewMenu->setObjectName(QString::fromUtf8("mViewMenu"));
    m3DMapViewsMenu = new QMenu(mViewMenu);
    m3DMapViewsMenu->setObjectName(QString::fromUtf8("m3DMapViewsMenu"));
    mMenuMeasure = new QMenu(mViewMenu);
    mMenuMeasure->setObjectName(QString::fromUtf8("mMenuMeasure"));
    mMenuDecorations = new QMenu(mViewMenu);
    mMenuDecorations->setObjectName(QString::fromUtf8("mMenuDecorations"));
    mMenuPreviewMode = new QMenu(mViewMenu);
    mMenuPreviewMode->setObjectName(QString::fromUtf8("mMenuPreviewMode"));
    mMenuLayerVisibility = new QMenu(mViewMenu);
    mMenuLayerVisibility->setObjectName(QString::fromUtf8("mMenuLayerVisibility"));
    mLayerMenu = new QMenu(menubar);
    mLayerMenu->setObjectName(QString::fromUtf8("mLayerMenu"));
    mNewLayerMenu = new QMenu(mLayerMenu);
    mNewLayerMenu->setObjectName(QString::fromUtf8("mNewLayerMenu"));
    mAddLayerMenu = new QMenu(mLayerMenu);
    mAddLayerMenu->setObjectName(QString::fromUtf8("mAddLayerMenu"));
    mMenuFilterTable = new QMenu(mLayerMenu);
    mMenuFilterTable->setObjectName(QString::fromUtf8("mMenuFilterTable"));
    mPluginMenu = new QMenu(menubar);
    mPluginMenu->setObjectName(QString::fromUtf8("mPluginMenu"));
    mHelpMenu = new QMenu(menubar);
    mHelpMenu->setObjectName(QString::fromUtf8("mHelpMenu"));
    mMenuPluginHelp = new QMenu(mHelpMenu);
    mMenuPluginHelp->setObjectName(QString::fromUtf8("mMenuPluginHelp"));
    mSettingsMenu = new QMenu(menubar);
    mSettingsMenu->setObjectName(QString::fromUtf8("mSettingsMenu"));
    mRasterMenu = new QMenu(menubar);
    mRasterMenu->setObjectName(QString::fromUtf8("mRasterMenu"));
    mVectorMenu = new QMenu(menubar);
    mVectorMenu->setObjectName(QString::fromUtf8("mVectorMenu"));
    mEditMenu = new QMenu(menubar);
    mEditMenu->setObjectName(QString::fromUtf8("mEditMenu"));
    mMenuSelect = new QMenu(mEditMenu);
    mMenuSelect->setObjectName(QString::fromUtf8("mMenuSelect"));
    mMenuPasteAs = new QMenu(mEditMenu);
    mMenuPasteAs->setObjectName(QString::fromUtf8("mMenuPasteAs"));
    mMenuAnnotation = new QMenu(mEditMenu);
    mMenuAnnotation->setObjectName(QString::fromUtf8("mMenuAnnotation"));
    mMenuEditAttributes = new QMenu(mEditMenu);
    mMenuEditAttributes->setObjectName(QString::fromUtf8("mMenuEditAttributes"));
    mMenuEditGeometry = new QMenu(mEditMenu);
    mMenuEditGeometry->setObjectName(QString::fromUtf8("mMenuEditGeometry"));
    mMeshMenu = new QMenu(menubar);
    mMeshMenu->setObjectName(QString::fromUtf8("mMeshMenu"));

    menubar->addAction(mProjectMenu->menuAction());
    menubar->addAction(mEditMenu->menuAction());
    menubar->addAction(mViewMenu->menuAction());
    menubar->addAction(mLayerMenu->menuAction());
    menubar->addAction(mSettingsMenu->menuAction());
    menubar->addAction(mPluginMenu->menuAction());
    menubar->addAction(mVectorMenu->menuAction());
    menubar->addAction(mRasterMenu->menuAction());
    menubar->addAction(mMeshMenu->menuAction());
    menubar->addAction(mHelpMenu->menuAction());
    mProjectMenu->addAction(mActionNewProject);
    mProjectMenu->addAction(mProjectFromTemplateMenu->menuAction());
    mProjectMenu->addSeparator();
    mProjectMenu->addAction(mActionOpenProject);
    mProjectMenu->addAction(mProjectFromStorageMenu->menuAction());
    mProjectMenu->addAction(mRecentProjectsMenu->menuAction());
    mProjectMenu->addSeparator();
    mProjectMenu->addAction(mActionCloseProject);
    mProjectMenu->addSeparator();
    mProjectMenu->addAction(mActionSaveProject);
    mProjectMenu->addAction(mActionSaveProjectAs);
    mProjectMenu->addAction(mProjectToStorageMenu->menuAction());
    mProjectMenu->addAction(mActionRevertProject);
    mProjectMenu->addSeparator();
    mProjectMenu->addAction(mActionProjectProperties);
    mProjectMenu->addAction(mActionSnappingOptions);
    mProjectMenu->addAction(menuImport_Export->menuAction());
    mProjectMenu->addSeparator();
    mProjectMenu->addAction(mActionNewPrintLayout);
    mProjectMenu->addAction(mActionNewReport);
    mProjectMenu->addAction(mActionShowLayoutManager);
    mProjectMenu->addAction(mLayoutsMenu->menuAction());
    mProjectMenu->addSeparator();
    mProjectMenu->addAction(mActionExit);
    menuImport_Export->addAction(mActionSaveMapAsImage);
    menuImport_Export->addAction(mActionSaveMapAsPdf);
    menuImport_Export->addAction(mActionDxfExport);
    menuImport_Export->addSeparator();
    menuImport_Export->addAction(mActionDwgImport);
    mViewMenu->addAction(mActionNewMapCanvas);
    mViewMenu->addAction(m3DMapViewsMenu->menuAction());
    mViewMenu->addAction(mActionPan);
    mViewMenu->addAction(mActionPanToSelected);
    mViewMenu->addAction(mActionZoomIn);
    mViewMenu->addAction(mActionZoomOut);
    mViewMenu->addSeparator();
    mViewMenu->addAction(mActionIdentify);
    mViewMenu->addAction(mMenuMeasure->menuAction());
    mViewMenu->addAction(mActionStatisticalSummary);
    mViewMenu->addAction(mActionElevationProfile);
    mViewMenu->addSeparator();
    mViewMenu->addAction(mActionZoomFullExtent);
    mViewMenu->addAction(mActionZoomToSelected);
    mViewMenu->addAction(mActionZoomToLayers);
    mViewMenu->addAction(mActionZoomActualSize);
    mViewMenu->addAction(mActionZoomLast);
    mViewMenu->addAction(mActionZoomNext);
    mViewMenu->addSeparator();
    mViewMenu->addAction(mMenuDecorations->menuAction());
    mViewMenu->addAction(mMenuPreviewMode->menuAction());
    mViewMenu->addAction(mActionMapTips);
    mViewMenu->addAction(mActionNewBookmark);
    mViewMenu->addAction(mActionShowBookmarks);
    mViewMenu->addAction(mActionShowBookmarkManager);
    mViewMenu->addAction(mActionDraw);
    mViewMenu->addSeparator();
    mViewMenu->addAction(mMenuLayerVisibility->menuAction());
    m3DMapViewsMenu->addAction(mActionNew3DMapCanvas);
    m3DMapViewsMenu->addAction(mActionManage3DMapViews);
    mMenuMeasure->addAction(mActionMeasure);
    mMenuMeasure->addAction(mActionMeasureArea);
    mMenuMeasure->addAction(mActionMeasureBearing);
    mMenuMeasure->addAction(mActionMeasureAngle);
    mMenuDecorations->addAction(mActionDecorationGrid);
    mMenuDecorations->addAction(mActionDecorationScaleBar);
    mMenuDecorations->addAction(mActionDecorationImage);
    mMenuDecorations->addAction(mActionDecorationNorthArrow);
    mMenuDecorations->addAction(mActionDecorationTitle);
    mMenuDecorations->addAction(mActionDecorationCopyright);
    mMenuDecorations->addAction(mActionDecorationLayoutExtent);
    mMenuPreviewMode->addAction(mActionPreviewModeOff);
    mMenuPreviewMode->addAction(mActionPreviewModeMono);
    mMenuPreviewMode->addAction(mActionPreviewModeGrayscale);
    mMenuPreviewMode->addAction(mActionPreviewProtanope);
    mMenuPreviewMode->addAction(mActionPreviewDeuteranope);
    mMenuPreviewMode->addAction(mActionPreviewTritanope);
    mMenuLayerVisibility->addAction(mActionShowAllLayers);
    mMenuLayerVisibility->addAction(mActionHideAllLayers);
    mMenuLayerVisibility->addAction(mActionShowSelectedLayers);
    mMenuLayerVisibility->addAction(mActionHideSelectedLayers);
    mMenuLayerVisibility->addAction(mActionToggleSelectedLayers);
    mMenuLayerVisibility->addAction(mActionToggleSelectedLayersIndependently);
    mMenuLayerVisibility->addAction(mActionHideDeselectedLayers);
    mLayerMenu->addAction(mActionDataSourceManager);
    mLayerMenu->addAction(mNewLayerMenu->menuAction());
    mLayerMenu->addAction(mAddLayerMenu->menuAction());
    mLayerMenu->addAction(mActionEmbedLayers);
    mLayerMenu->addAction(mActionAddLayerDefinition);
    mLayerMenu->addAction(mActionShowGeoreferencer);
    mLayerMenu->addSeparator();
    mLayerMenu->addAction(mActionCopyStyle);
    mLayerMenu->addAction(mActionPasteStyle);
    mLayerMenu->addSeparator();
    mLayerMenu->addAction(mActionCopyLayer);
    mLayerMenu->addAction(mActionPasteLayer);
    mLayerMenu->addSeparator();
    mLayerMenu->addAction(mActionOpenTable);
    mLayerMenu->addAction(mMenuFilterTable->menuAction());
    mLayerMenu->addAction(mActionToggleEditing);
    mLayerMenu->addAction(mActionSaveLayerEdits);
    mLayerMenu->addAction(mActionAllEdits);
    mLayerMenu->addSeparator();
    mLayerMenu->addAction(mActionLayerSaveAs);
    mLayerMenu->addAction(mActionSaveLayerDefinition);
    mLayerMenu->addAction(mActionRemoveLayer);
    mLayerMenu->addAction(mActionDuplicateLayer);
    mLayerMenu->addAction(mActionSetLayerScaleVisibility);
    mLayerMenu->addAction(mActionSetLayerCRS);
    mLayerMenu->addAction(mActionSetProjectCRSFromLayer);
    mLayerMenu->addAction(mActionLayerProperties);
    mLayerMenu->addAction(mActionLayerSubsetString);
   // mLayerMenu->addAction(mActionLabeling); // SG Add
    mLayerMenu->addSeparator();
    mLayerMenu->addAction(mActionAddToOverview);
    mLayerMenu->addAction(mActionAddAllToOverview);
    mLayerMenu->addAction(mActionRemoveAllFromOverview);
    mNewLayerMenu->addAction(mActionNewGeoPackageLayer);
    mNewLayerMenu->addAction(mActionNewVectorLayer);
    mNewLayerMenu->addAction(mActionNewSpatiaLiteLayer);
    mNewLayerMenu->addAction(mActionNewMemoryLayer);
    mNewLayerMenu->addAction(mActionNewMeshLayer);
    mNewLayerMenu->addAction(mActionNewGpxLayer);
    mNewLayerMenu->addAction(mActionNewVirtualLayer);
    mAddLayerMenu->addAction(mActionAddOgrLayer);
    mAddLayerMenu->addAction(mActionAddRasterLayer);
    mAddLayerMenu->addAction(mActionAddMeshLayer);
    mAddLayerMenu->addAction(mActionAddDelimitedText);
    mAddLayerMenu->addAction(mActionAddPgLayer);
    mAddLayerMenu->addAction(mActionAddSpatiaLiteLayer);
    mAddLayerMenu->addAction(mActionAddMssqlLayer);
    mAddLayerMenu->addAction(mActionAddOracleLayer);
    mAddLayerMenu->addAction(mActionAddHanaLayer);
    mAddLayerMenu->addAction(mActionAddVirtualLayer);
    mAddLayerMenu->addAction(mActionAddWmsLayer);
    mAddLayerMenu->addAction(mActionAddXyzLayer);
    mAddLayerMenu->addAction(mActionAddLayerSeparator);
    mAddLayerMenu->addAction(mActionAddWcsLayer);
    mAddLayerMenu->addAction(mActionAddWfsLayer);
    mAddLayerMenu->addAction(mActionAddAfsLayer);
    mAddLayerMenu->addAction(mActionAddVectorTileLayer);
    mAddLayerMenu->addAction(mActionAddPointCloudLayer);
    mMenuFilterTable->addAction(mActionOpenTableSelected);
    mMenuFilterTable->addAction(mActionOpenTableVisible);
    mMenuFilterTable->addAction(mActionOpenTableEdited);
    mPluginMenu->addAction(mActionManagePlugins);
    mPluginMenu->addSeparator();
    mPluginMenu->addAction(mActionShowPythonDialog);
    mHelpMenu->addAction(mActionHelpContents);
    mHelpMenu->addAction(mActionHelpAPI);
    mHelpMenu->addSeparator();
    mHelpMenu->addAction(mMenuPluginHelp->menuAction());
    mHelpMenu->addSeparator();
    mHelpMenu->addAction(mActionReportaBug);
    mHelpMenu->addAction(mActionNeedSupport);
    mHelpMenu->addSeparator();
    mHelpMenu->addAction(mActionQgisHomePage);
    mHelpMenu->addAction(mActionCheckQgisVersion);
    mHelpMenu->addSeparator();
    mHelpMenu->addAction(mActionAbout);
    mHelpMenu->addAction(mActionSponsors);
    mSettingsMenu->addAction(mActionStyleManager);
    mSettingsMenu->addAction(mActionCustomProjection);
    mSettingsMenu->addSeparator();
    mSettingsMenu->addAction(mActionConfigureShortcuts);
    mSettingsMenu->addAction(mActionCustomization);
    mSettingsMenu->addSeparator();
    mSettingsMenu->addAction(mActionOptions);
    mRasterMenu->addAction(mActionShowRasterCalculator);
    mRasterMenu->addAction(mActionShowAlignRasterTool);
    mEditMenu->addAction(mActionUndo);
    mEditMenu->addAction(mActionRedo);
    mEditMenu->addSeparator();
    mEditMenu->addAction(mActionCutFeatures);
    mEditMenu->addAction(mActionCopyFeatures);
    mEditMenu->addAction(mActionPasteFeatures);
    mEditMenu->addAction(mMenuPasteAs->menuAction());
    mEditMenu->addAction(mActionDeleteSelected);
    mEditMenu->addAction(mMenuSelect->menuAction());
    mEditMenu->addSeparator();
    mEditMenu->addAction(mActionAddFeature);
    mEditMenu->addAction(mMenuAnnotation->menuAction());
    mEditMenu->addSeparator();
    mEditMenu->addAction(mMenuEditAttributes->menuAction());
    mEditMenu->addAction(mMenuEditGeometry->menuAction());
    mEditMenu->addAction(mActionRotatePointSymbols);
    mEditMenu->addAction(mActionOffsetPointSymbol);
    mMenuSelect->addAction(mActionSelectFeatures);
    mMenuSelect->addAction(mActionSelectPolygon);
    mMenuSelect->addAction(mActionSelectFreehand);
    mMenuSelect->addAction(mActionSelectRadius);
    mMenuSelect->addAction(mActionSelectByForm);
    mMenuSelect->addAction(mActionSelectByExpression);
    mMenuSelect->addAction(mActionDeselectAll);
    mMenuSelect->addAction(mActionDeselectActiveLayer);
    mMenuSelect->addAction(mActionReselect);
    mMenuSelect->addAction(mActionSelectAll);
    mMenuSelect->addAction(mActionInvertSelection);
    mMenuPasteAs->addAction(mActionPasteAsNewVector);
    mMenuPasteAs->addAction(mActionPasteAsNewMemoryVector);
    mMenuAnnotation->addAction(mActionTextAnnotation);
    mMenuAnnotation->addAction(mActionFormAnnotation);
    mMenuAnnotation->addAction(mActionHtmlAnnotation);
    mMenuAnnotation->addAction(mActionSvgAnnotation);
    mMenuEditAttributes->addAction(mActionMultiEditAttributes);
    mMenuEditAttributes->addAction(mActionMergeFeatureAttributes);
    mMenuEditGeometry->addAction(mActionMoveFeature);
    mMenuEditGeometry->addAction(mActionMoveFeatureCopy);
    mMenuEditGeometry->addAction(mActionRotateFeature);
    mMenuEditGeometry->addAction(mActionScaleFeature);
    mMenuEditGeometry->addAction(mActionSimplifyFeature);
    mMenuEditGeometry->addAction(mActionAddRing);
    mMenuEditGeometry->addAction(mActionAddPart);
    mMenuEditGeometry->addAction(mActionFillRing);
    mMenuEditGeometry->addAction(mActionDeleteRing);
    mMenuEditGeometry->addAction(mActionDeletePart);
    mMenuEditGeometry->addAction(mActionReshapeFeatures);
    mMenuEditGeometry->addAction(mActionOffsetCurve);
    mMenuEditGeometry->addAction(mActionSplitFeatures);
    mMenuEditGeometry->addAction(mActionSplitParts);
    mMenuEditGeometry->addAction(mActionMergeFeatures);
    mMenuEditGeometry->addAction(mActionVertexTool);
    mMenuEditGeometry->addAction(mActionReverseLine);
    mMenuEditGeometry->addAction(mActionTrimExtendFeature);
    mMeshMenu->addAction(mActionShowMeshCalculator);

    mProjectMenu->setTitle(QCoreApplication::translate("MainWindow", "Pro&ject", nullptr));
    mRecentProjectsMenu->setTitle(QCoreApplication::translate("MainWindow", "Open &Recent", nullptr));
    mLayoutsMenu->setTitle(QCoreApplication::translate("MainWindow", "Layouts", nullptr));
    mProjectFromTemplateMenu->setTitle(QCoreApplication::translate("MainWindow", "New from Template", nullptr));
    mProjectToStorageMenu->setTitle(QCoreApplication::translate("MainWindow", "Save To", nullptr));
    mProjectFromStorageMenu->setTitle(QCoreApplication::translate("MainWindow", "Open From", nullptr));
    menuImport_Export->setTitle(QCoreApplication::translate("MainWindow", "Import/Export", nullptr));
    mViewMenu->setTitle(QCoreApplication::translate("MainWindow", "&View", nullptr));
    m3DMapViewsMenu->setTitle(QCoreApplication::translate("MainWindow", "&3D Map Views", nullptr));
    mMenuMeasure->setTitle(QCoreApplication::translate("MainWindow", "Measure", nullptr));
    mMenuDecorations->setTitle(QCoreApplication::translate("MainWindow", "&Decorations", nullptr));
    mMenuPreviewMode->setTitle(QCoreApplication::translate("MainWindow", "Preview Mode", nullptr));
    mMenuLayerVisibility->setTitle(QCoreApplication::translate("MainWindow", "Layer Visibility", nullptr));
    mLayerMenu->setTitle(QCoreApplication::translate("MainWindow", "&Layer", nullptr));
    mNewLayerMenu->setTitle(QCoreApplication::translate("MainWindow", "Create Layer", nullptr));
    mAddLayerMenu->setTitle(QCoreApplication::translate("MainWindow", "Add Layer", nullptr));
    mMenuFilterTable->setTitle(QCoreApplication::translate("MainWindow", "Filter Attribute Table", nullptr));
    mPluginMenu->setTitle(QCoreApplication::translate("MainWindow", "&Plugins", nullptr));
    mHelpMenu->setTitle(QCoreApplication::translate("MainWindow", "&Help", nullptr));
    mMenuPluginHelp->setTitle(QCoreApplication::translate("MainWindow", "Plugins", nullptr));
    mSettingsMenu->setTitle(QCoreApplication::translate("MainWindow", "&Settings", nullptr));
    mRasterMenu->setTitle(QCoreApplication::translate("MainWindow", "&Raster", nullptr));
    mVectorMenu->setTitle(QCoreApplication::translate("MainWindow", "Vect&or", nullptr));
    mEditMenu->setTitle(QCoreApplication::translate("MainWindow", "&Edit", nullptr));
    mMenuSelect->setTitle(QCoreApplication::translate("MainWindow", "Select", nullptr));
    mMenuPasteAs->setTitle(QCoreApplication::translate("MainWindow", "Paste Features As", nullptr));
    mMenuAnnotation->setTitle(QCoreApplication::translate("MainWindow", "Add Annotation", nullptr));
    mMenuEditAttributes->setTitle(QCoreApplication::translate("MainWindow", "Edit Attributes", nullptr));
    mMenuEditGeometry->setTitle(QCoreApplication::translate("MainWindow", "Edit Geometry", nullptr));
    mMeshMenu->setTitle(QCoreApplication::translate("MainWindow", "&Mesh", nullptr));
}


void QgisApp::QgisApp::setupToolBars()
{
    mFileToolBar = new QToolBar(this);
    mFileToolBar->setObjectName(QString::fromUtf8("mFileToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mFileToolBar);
    mLayerToolBar = new QToolBar(this);
    mLayerToolBar->setObjectName(QString::fromUtf8("mLayerToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mLayerToolBar);
    mDigitizeToolBar = new QToolBar(this);
    mDigitizeToolBar->setObjectName(QString::fromUtf8("mDigitizeToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mDigitizeToolBar);
//    this->insertToolBarBreak(mDigitizeToolBar);
    mAdvancedDigitizeToolBar = new QToolBar(this);
    mAdvancedDigitizeToolBar->setObjectName(QString::fromUtf8("mAdvancedDigitizeToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mAdvancedDigitizeToolBar);
    mMapNavToolBar = new QToolBar(this);
    mMapNavToolBar->setObjectName(QString::fromUtf8("mMapNavToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mMapNavToolBar);
    mAttributesToolBar = new QToolBar(this);
    mAttributesToolBar->setObjectName(QString::fromUtf8("mAttributesToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mAttributesToolBar);
//    this->insertToolBarBreak(mAttributesToolBar);
    mPluginToolBar = new QToolBar(this);
    mPluginToolBar->setObjectName(QString::fromUtf8("mPluginToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mPluginToolBar);
    mHelpToolBar = new QToolBar(this);
    mHelpToolBar->setObjectName(QString::fromUtf8("mHelpToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mHelpToolBar);
    mRasterToolBar = new QToolBar(this);
    mRasterToolBar->setObjectName(QString::fromUtf8("mRasterToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mRasterToolBar);
    mLabelToolBar = new QToolBar(this);
    mLabelToolBar->setObjectName(QString::fromUtf8("mLabelToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mLabelToolBar);
    mVectorToolBar = new QToolBar(this);
    mVectorToolBar->setObjectName(QString::fromUtf8("mVectorToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mVectorToolBar);
    mDatabaseToolBar = new QToolBar(this);
    mDatabaseToolBar->setObjectName(QString::fromUtf8("mDatabaseToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mDatabaseToolBar);
    mWebToolBar = new QToolBar(this);
    mWebToolBar->setObjectName(QString::fromUtf8("mWebToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mWebToolBar);
    mSnappingToolBar = new QToolBar(this);
    mSnappingToolBar->setObjectName(QString::fromUtf8("mSnappingToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mSnappingToolBar);
    mDataSourceManagerToolBar = new QToolBar(this);
    mDataSourceManagerToolBar->setObjectName(QString::fromUtf8("mDataSourceManagerToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mDataSourceManagerToolBar);
    mShapeDigitizeToolBar = new QToolBar(this);
    mShapeDigitizeToolBar->setObjectName(QString::fromUtf8("mShapeDigitizeToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mShapeDigitizeToolBar);
    mSelectionToolBar = new QToolBar(this);
    mSelectionToolBar->setObjectName(QString::fromUtf8("mSelectionToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mSelectionToolBar);
    mMeshToolBar = new QToolBar(this);
    mMeshToolBar->setObjectName(QString::fromUtf8("mMeshToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mMeshToolBar);
    mAnnotationsToolBar = new QToolBar(this);
    mAnnotationsToolBar->setObjectName(QString::fromUtf8("mAnnotationsToolBar"));
//    this->addToolBar(Qt::TopToolBarArea, mAnnotationsToolBar);

    statusbar = new QStatusBar(this);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    this->setStatusBar(statusbar);

    mFileToolBar->addAction(mActionNewProject);
    mFileToolBar->addAction(mActionOpenProject);
    mFileToolBar->addAction(mActionSaveProject);
    mFileToolBar->addAction(mActionNewPrintLayout);
    mFileToolBar->addAction(mActionShowLayoutManager);
    mFileToolBar->addAction(mActionStyleManager);
    mLayerToolBar->addAction(mActionAddOgrLayer);
    mLayerToolBar->addAction(mActionAddRasterLayer);
    mLayerToolBar->addAction(mActionAddMeshLayer);
    mLayerToolBar->addAction(mActionAddDelimitedText);
    mLayerToolBar->addAction(mActionAddSpatiaLiteLayer);
    mLayerToolBar->addAction(mActionAddVirtualLayer);
    mLayerToolBar->addAction(mActionAddWmsLayer);
    mLayerToolBar->addAction(mActionAddWcsLayer);
    mLayerToolBar->addAction(mActionAddWfsLayer);
    mDigitizeToolBar->addAction(mActionAllEdits);
    mDigitizeToolBar->addAction(mActionToggleEditing);
    mDigitizeToolBar->addAction(mActionSaveLayerEdits);
    mDigitizeToolBar->addAction(mActionAddFeature);
    mDigitizeToolBar->addAction(mActionMultiEditAttributes);
    mDigitizeToolBar->addAction(mActionDeleteSelected);
    mDigitizeToolBar->addAction(mActionCutFeatures);
    mDigitizeToolBar->addAction(mActionCopyFeatures);
    mDigitizeToolBar->addAction(mActionPasteFeatures);
    mDigitizeToolBar->addAction(mActionUndo);
    mDigitizeToolBar->addAction(mActionRedo);
    mAdvancedDigitizeToolBar->addAction(mActionRotateFeature);
    mAdvancedDigitizeToolBar->addAction(mActionScaleFeature);
    mAdvancedDigitizeToolBar->addAction(mActionSimplifyFeature);
    mAdvancedDigitizeToolBar->addAction(mActionAddRing);
    mAdvancedDigitizeToolBar->addAction(mActionAddPart);
    mAdvancedDigitizeToolBar->addAction(mActionFillRing);
    mAdvancedDigitizeToolBar->addAction(mActionDeleteRing);
    mAdvancedDigitizeToolBar->addAction(mActionDeletePart);
    mAdvancedDigitizeToolBar->addAction(mActionReshapeFeatures);
    mAdvancedDigitizeToolBar->addAction(mActionOffsetCurve);
    mAdvancedDigitizeToolBar->addAction(mActionReverseLine);
    mAdvancedDigitizeToolBar->addAction(mActionTrimExtendFeature);
    mAdvancedDigitizeToolBar->addAction(mActionSplitFeatures);
    mAdvancedDigitizeToolBar->addAction(mActionSplitParts);
    mAdvancedDigitizeToolBar->addAction(mActionMergeFeatures);
    mAdvancedDigitizeToolBar->addAction(mActionMergeFeatureAttributes);
    mMapNavToolBar->addAction(mActionPan);
    mMapNavToolBar->addAction(mActionPanToSelected);
    mMapNavToolBar->addAction(mActionZoomIn);
    mMapNavToolBar->addAction(mActionZoomOut);
    mMapNavToolBar->addAction(mActionZoomFullExtent);
    mMapNavToolBar->addAction(mActionZoomToSelected);
    mMapNavToolBar->addAction(mActionZoomToLayers);
    mMapNavToolBar->addAction(mActionZoomActualSize);
    mMapNavToolBar->addAction(mActionZoomLast);
    mMapNavToolBar->addAction(mActionZoomNext);
    mMapNavToolBar->addAction(mActionNewMapCanvas);
    mMapNavToolBar->addAction(mActionNew3DMapCanvas);
    mMapNavToolBar->addAction(mActionNewBookmark);
    mMapNavToolBar->addAction(mActionShowBookmarks);
    mMapNavToolBar->addAction(mActionTemporalController);
    mMapNavToolBar->addAction(mActionDraw);
    mAttributesToolBar->addAction(mActionIdentify);
    mAttributesToolBar->addAction(mActionOpenFieldCalc);
    mAttributesToolBar->addAction(mActionStatisticalSummary);
    mAttributesToolBar->addAction(mActionMapTips);
    mPluginToolBar->addAction(mActionShowPythonDialog);
    mHelpToolBar->addAction(mActionHelpContents);
    mRasterToolBar->addAction(mActionLocalCumulativeCutStretch);
    mRasterToolBar->addAction(mActionFullCumulativeCutStretch);
    mRasterToolBar->addAction(mActionLocalHistogramStretch);
    mRasterToolBar->addAction(mActionFullHistogramStretch);
    mRasterToolBar->addAction(mActionIncreaseBrightness);
    mRasterToolBar->addAction(mActionDecreaseBrightness);
    mRasterToolBar->addAction(mActionIncreaseContrast);
    mRasterToolBar->addAction(mActionDecreaseContrast);
    mRasterToolBar->addAction(mActionIncreaseGamma);
    mRasterToolBar->addAction(mActionDecreaseGamma);
    mLabelToolBar->addAction(mActionLabeling);
    mLabelToolBar->addAction(mActionDiagramProperties);
    mLabelToolBar->addSeparator();
    mLabelToolBar->addAction(mActionShowPinnedLabels);
    mLabelToolBar->addAction(mActionShowUnplacedLabels);
    mLabelToolBar->addSeparator();
    mLabelToolBar->addAction(mActionPinLabels);
    mLabelToolBar->addAction(mActionShowHideLabels);
    mLabelToolBar->addAction(mActionMoveLabel);
    mLabelToolBar->addAction(mActionRotateLabel);
    mLabelToolBar->addAction(mActionChangeLabelProperties);
    mDataSourceManagerToolBar->addAction(mActionDataSourceManager);
    mDataSourceManagerToolBar->addAction(mActionNewGeoPackageLayer);
    mDataSourceManagerToolBar->addAction(mActionNewVectorLayer);
    mDataSourceManagerToolBar->addAction(mActionNewSpatiaLiteLayer);
    mDataSourceManagerToolBar->addAction(mActionNewMemoryLayer);
    mDataSourceManagerToolBar->addAction(mActionNewMeshLayer);
    mDataSourceManagerToolBar->addAction(mActionNewVirtualLayer);
    mAnnotationsToolBar->addAction(mActionModifyAnnotation);

#if QT_CONFIG(tooltip)
        statusbar->setToolTip(QCoreApplication::translate("MainWindow", "Statusbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mFileToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Project Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mFileToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Project Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mLayerToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Manage Layers Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mLayerToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Manage Layers Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mDigitizeToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Digitizing Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mDigitizeToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Digitizing Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mAdvancedDigitizeToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Advanced Digitizing Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mAdvancedDigitizeToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Advanced Digitizing Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mMapNavToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Map Navigation Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mMapNavToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Map Navigation Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mAttributesToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Attributes Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mAttributesToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Attributes Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mPluginToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Plugins Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mPluginToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Plugins Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mHelpToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Help Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mHelpToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Help Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mRasterToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Raster Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mRasterToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Raster Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mLabelToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Label Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mLabelToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Label Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mVectorToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Vector Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mVectorToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Vector Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mDatabaseToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Database Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mDatabaseToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Database Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mWebToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Web Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mWebToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Web Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mSnappingToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Snapping Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mSnappingToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Snapping Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mDataSourceManagerToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Data Source Manager Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mDataSourceManagerToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Data Source Manager Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mShapeDigitizeToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Shape Digitizing Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mShapeDigitizeToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Shape Digitizing Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mSelectionToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Selection Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mSelectionToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Selection Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mMeshToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Mesh Digitizing Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mMeshToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Mesh Digitizing Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)
        mAnnotationsToolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "Annotations Toolbar", nullptr));
#if QT_CONFIG(tooltip)
        mAnnotationsToolBar->setToolTip(QCoreApplication::translate("MainWindow", "Annotations Toolbar", nullptr));
#endif // QT_CONFIG(tooltip)

}
