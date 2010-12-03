<?php
// $Id$

/* 
 * @file whats_new_block.inc
 */


function whats_new_block($numResults=5) {
 
    global $base_url;
  
    module_load_include('php', 'islandora_solr_search', 'Solr/Service');
    $host = variable_get('islandora_solr_search_block_host','localhost');
    $port = variable_get('islandora_solr_search_block_port','8080');
    $appName = variable_get('islandora_solr_search_block_app_name','solr');
    
    $query = 'created_d:[* TO *]';
    $queryURL = 'http://' . $host . ':' . $port . '/' . $appName . '/select?q='.drupal_urlencode($query).'&sort=created_d%20desc&start=0&rows='.$numResults;
    
    $cache_key = 'fedora_whats_new_block_'.md5($queryURL);
    if (($output = cache_get($cache_key))===0)  
    {
      try {
	$results = file_get_contents($queryURL);
      }catch (Exception $e) {
	drupal_set_message(t('error searching ').$e->getMessage());
      }
      if(empty($results)){
	drupal_set_message(t('Error searching solr index. Is the solr search block configured properly?'),'error');
	return ' ';
      }  
      
      if (trim($results) == '') {
	return NULL;
      }

      try {
	$proc = new XsltProcessor();
      } catch (Exception $e) {
	drupal_set_message($e->getMessage(), 'error');
	return;
      }

      $path = drupal_get_path('module','fedora_repository');
      $proc->setParameter('', 'baseUrl', $base_url);
      $proc->setParameter('', 'path', $base_url . '/' . $path);
      $proc->registerPHPFunctions();
      $input = NULL;
      $xsl = new DomDocument();
      try {
	$xsl->load(drupal_get_path('module','fedora_image_blocks') . '/xsl/whatsNew.xsl');
	$input = new DomDocument();
	$input->loadXML(trim($results));
	$xsl = $proc->importStylesheet($xsl);
	$newdom = $proc->transformToDoc($input);
	
	$output = ($newdom!==FALSE)?$newdom->saveXML():'';

	
      } catch (exception $e) {
	watchdog(t("Fedora_Repository"), t("Problem loading XSL file: !e", array('!e' => $e)), NULL, WATCHDOG_ERROR);
      }
      $output = '<ul>'.$output.'</ul>';
      cache_set($cache_key,$output,'cache',time()+variable_get('fedora_image_blocks_cache_time',3600));
    }
    
    if (is_object($output))
    {
      $output=$output->data;
    }
    
    return $output;
}
