<?php
// $Id$

/* 
 * @file image_rotator_block.inc
 */


function randSort($a,$b)
{
	return rand(-1,1);
}

function get_random_images($numResults=1,$query= NULL) {
  module_load_include('inc', 'fedora_repository', 'CollectionClass');
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');
  
  $itqlquery = ($query != NULL)?$query:variable_get ('fedora_image_rotator_block_query', 'select $object $title $collection $collectionTitle from <#ri>
where $object <info:fedora/fedora-system:def/relations-external#isMemberOfCollection> <info:fedora/ilives:figures>
and <info:fedora/ilives:figures> <dc:identifier> $collection
and <info:fedora/ilives:figures> <dc:title> $collectionTitle
and $object <fedora-model:state> <info:fedora/fedora-system:def/model#Active>
and $object <dc:title> $title');

  

$collection = new CollectionClass(variable_get('fedora_image_rotator_collection','ilives:figures'));
  $results = $collection->getRelatedItems(variable_get('fedora_image_rotator_collection','ilives:figures'), $itqlquery);
  //drupal_set_message($results);

  
  if (trim($results) == '') {
    return NULL;
  }
  $resultsdoc = new DomDocument();
  $resultsdoc->loadXML($results);
  
  $resultslist = $resultsdoc->getElementsByTagName('result');
  if ( $resultslist->length == 0 ) {
    return NULL;
  }

	$indices = array(); 
  for ($i=0;$i<$resultslist->length;$i++) {
		$indices[]=$i;
	}
	usort($indices,'randSort');

  $results = array();
	for ($i=0;$i<	min($numResults,$resultslist->length); $i++) {
		$randresult=$resultslist->item($indices[$i]);

	  $objs = $randresult->getElementsByTagName('object');
  	$obj = $objs->item(0);
	  $pid = substr( $obj->getAttribute('uri'), 12);
  	$titles = $randresult->getElementsByTagName('title');
	  $title = $titles->item(0);

		$collectionTitle = $randresult->getElementsByTagName('collectionTitle');
		$collectionTitle = $collectionTitle->item(0)->nodeValue;

    $collectionPid = $randresult->getElementsByTagName('collection');
    $collectionPid = $collectionPid->item(0)->nodeValue;
    
    $dsid = $randresult->getElementsByTagName('dsid');
    if ($dsid->length > 0) {
      $dsid = $dsid->item(0)->nodeValue;
    } else 
    {
      $dsid = NULL;
    }

		$results[]= array('pid' => $pid,
               //'title' => $randresult->textContent);
               'title' => $title->nodeValue,
							 'collectionTitle' => $collectionTitle,
							 'collectionPid' => $collectionPid,
							 'dsid'=> $dsid!=NULL?$dsid:'JPG');
	}
	return $results;
}


function _fedora_image_rotator_block_content($query = NULL, $style = 'large') {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  if (variable_get('fedora_image_rotator_block_enabled',true)) {
	  module_load_include('inc', 'fedora_repository', 'plugins/tagging_form');
	}

  global $base_url;

  $numImages = variable_get('fedora_image_rotator_block_numimages',1); 
  if ($style == 'smallthree') { 
    $numImages=$numImages*3;
  }
  
  $cache_key = 'fedora_image_rotator_block_'.md5(variable_get('fedora_image_rotator_block_numimages',1).$query);
  if (($results = cache_get($cache_key)) === 0) {
    $results= get_random_images($numImages,$query);  
    cache_set($cache_key,$results,'cache',time()+variable_get('fedora_image_blocks_cache_time',3600));
  } else
  {
    $results= $results->data;
  }

  if ($style == 'smallthree') {
    
    $new=array();
    $tmp=array();
    for ($i=0;$i<count($results); $i++) 
    {
      if ($i != 0 && $i % 3 == 0) {
        $new[]=$tmp;
        $tmp=array();
      }
      $tmp[]=$results[$i];
    }
    if (count($tmp) > 0) { 
      $new[]=$tmp;
    }
    $results = $new;
    
  }


  $content = '';
  $pager = '';

  if ($results != NULL && count($results) > 0) {
    

  $path = drupal_get_path('module','fedora_image_blocks');
  drupal_add_css($path.'/css/rotator.css');

  if (count($results) == 1) {
    drupal_add_js(" $(document).ready(function () { $('ul.imageRotator li:first').show(); }); ",'inline');
  } else if (count($results) > 1) {
    drupal_add_js("
  $(document).ready(function () {

  $('ul.imageRotator li:first').show();
      
    $('ul.imageRotator li:first img').load(function () { 
	    $('div.imageRotator_pager a:first').addClass('active');
	  }); 
    var rotator = { timeout: null,
		    currIndex: 1,
		    maxIndex: ".count($results).",
		    speed: 400,
		    delay: 15000,
		    callback: function () {
			$('ul.imageRotator li:nth-child('+this.currIndex+')').hide();
			$('div.imageRotator_pager a#page'+this.currIndex).removeClass('active');
			this.currIndex++;
			if (this.currIndex>this.maxIndex) {
			    this.currIndex=1;
			}
			$('ul.imageRotator li:nth-child('+this.currIndex+')').fadeIn(this.speed);
											  
				  
											  $('div.imageRotator_pager a#page'+this.currIndex).addClass('active');						
			rotator.timeout = window.setTimeout(function () { rotator.callback(); }, rotator.delay);

		    }
		  };

    rotator.timeout = window.setTimeout(function () { rotator.callback(); }, rotator.delay);

	  $('div.imageRotator_pager a').click(function () { return false; } );
	  $('div.imageRotator_pager a').hover(function () { 
	  var index=  parseInt($(this).attr('id').substr(4));
		  if (index != rotator.currIndex)
		  {
			  $('div.imageRotator_pager a#page'+rotator.currIndex).removeClass('active');	
			  $(this).addClass('active');
			  $('ul.imageRotator li:nth-child('+rotator.currIndex+')').hide();
		    rotator.currIndex=index;	
			  $('ul.imageRotator li:nth-child('+rotator.currIndex+')').fadeIn(rotator.speed);
		  }
		  window.clearTimeout(rotator.timeout);

	  }, function () { 
		  window.clearTimeout(rotator.timeout); rotator.timeout = window.setTimeout(function () {  rotator.callback(); }, rotator.delay); 
		  
	  });

  });

  ",'inline');
}	  
	$content .= '<ul class="imageRotator">';
	$pager .='<div class="imageRotator_pager"/>';
	$i=1;
  
  $height = ($style == 'smallthree')?163:277;
  $width  = ($style == 'smallthree')?160:472;
  
	foreach ($results as $res) {	
    $content .= '<li>';
    
    if ($style == 'smallthree') {
      
      $img = '';
      foreach ($res as $result) {
        $img .= l('<img src="'. $base_url . '/fedora/imageapi/'. $result['pid'] .  '/'.$result['dsid'].'?op=centerscale&height='.$height.'&width='.$width.'" height="'.$height.'" width="'.$width.'"/>','fedora/repository/'. $result['pid'], array('html' => TRUE, 'alias' => TRUE));
      }
      
    } else {
      $result = $res;
      $img = l('<img src="'. $base_url . '/fedora/imageapi/'. $result['pid'] .  '/'.$result['dsid'].'?op=centerscale&height='.$height.'&width='.$width.'" height="'.$height.'" width="'.$width.'"/>','fedora/repository/'. $result['pid'], array('html' => TRUE, 'alias' => TRUE));
    }
    
    $content .= $img ;
    if ($style == 'large') {
      $content .='<div class="info"><h2>'. l($result['collectionTitle'], 'fedora/repository/'.$result['collectionPid'] , array('html' => TRUE, 'alias' => TRUE))  
      .'</h2><span>'.$result['title'].'</span>'
     . '<div class="more"><a href="'.$base_url .'/fedora/repository/'.$result['collectionPid'].'">more from this collection</a></div>'
     .'</div>';
    }
    $content .= '</li>';
		$pager .= '<a href="'.$base_url.'/fedora/repository/'.$result['pid'].'" id="page'.$i.'">'.$i.'</a>';
		$i++;
	}		
	$content .='</ul>';
	$pager.='</div>';
  } 

	return $content. (count($results) > 1?$pager:'');
}
